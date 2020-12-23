#include "../ethos.hpp"
Interface* interface = nullptr;

void Interface::loadRequest(unsigned id, string name, string type) {
  return utility->loadRequest(id, name, type);
}

void Interface::loadRequest(unsigned id, string path) {
  return utility->loadRequest(id, path);
}

void Interface::saveRequest(unsigned id, string path) {
  return utility->saveRequest(id, path);
}

uint32_t Interface::videoColor(unsigned source, uint16_t a, uint16_t r, uint16_t g, uint16_t b) {
  if(config->video.shader != "Display Emulation") {
    if(config->video.saturation != 100) {
      uint16_t grayscale = uclamp<16>((r + g + b) / 3);
      double saturation = config->video.saturation * 0.01;
      double inverse = max(0.0, 1.0 - saturation);
      r = uclamp<16>(r * saturation + grayscale * inverse);
      g = uclamp<16>(g * saturation + grayscale * inverse);
      b = uclamp<16>(b * saturation + grayscale * inverse);
    }

    if(config->video.gamma != 100) {
      double exponent = config->video.gamma * 0.01;
      double reciprocal = 1.0 / 32767.0;
      r = r > 32767 ? r : 32767 * pow(r * reciprocal, exponent);
      g = g > 32767 ? g : 32767 * pow(g * reciprocal, exponent);
      b = b > 32767 ? b : 32767 * pow(b * reciprocal, exponent);
    }

    if(config->video.luminance != 100) {
      double luminance = config->video.luminance * 0.01;
      r = r * luminance;
      g = g * luminance;
      b = b * luminance;
    }
  }

  if(program->depth == 30) {
    a >>= 14, r >>= 6, g >>= 6, b >>= 6;
    return a << 30 | r << 20 | g << 10 | b << 0;
  }

  if(program->depth == 24) {
    a >>= 8, r >>= 8, g >>= 8, b >>= 8;
    return a << 24 | r << 16 | g << 8 | b << 0;
  }

  return 0u;
}

void Interface::videoRefresh(const uint32_t* palette, const uint32_t* data, unsigned pitch, unsigned width, unsigned height) {
  uint32_t* output;
  unsigned outputPitch;

  if(video.lock(output, outputPitch, width, height)) {
    pitch >>= 2, outputPitch >>= 2;

    for(unsigned y = 0; y < height; y++) {
      const uint32_t* sp = data + y * pitch;
      uint32_t* dp = output + y * outputPitch;
      for(unsigned x = 0; x < width; x++) {
        *dp++ = palette[*sp++];
      }
    }

    if(system().information.overscan && config->video.maskOverscan.enable) {
      unsigned h = config->video.maskOverscan.horizontal;
      unsigned v = config->video.maskOverscan.vertical;

      if(h) for(unsigned y = 0; y < height; y++) {
        memset(output + y * outputPitch, 0, 4 * h);
        memset(output + y * outputPitch + (width - h), 0, 4 * h);
      }

      if(v) for(unsigned y = 0; y < v; y++) {
        memset(output + y * outputPitch, 0, 4 * width);
        memset(output + (height - 1 - y) * outputPitch, 0, 4 * width);
      }
    }

    video.unlock();
    video.refresh();
  }

  static unsigned frameCounter = 0;
  static time_t previous, current;
  frameCounter++;

  time(&current);
  if(current != previous) {
    previous = current;
    utility->setStatusText({"FPS: ", frameCounter});
    frameCounter = 0;
  }
}

void Interface::audioSample(int16_t lsample, int16_t rsample) {
  signed samples[] = {lsample, rsample};
  dspaudio.sample(samples);
  while(dspaudio.pending()) {
    dspaudio.read(samples);
    audio.sample(samples[0], samples[1]);
  }
}

int16_t Interface::inputPoll(unsigned port, unsigned device, unsigned input) {
  unsigned guid = system().port[port].device[device].input[input].guid;
  return inputManager->inputMap[guid]->poll();
}

void Interface::inputRumble(unsigned port, unsigned device, unsigned input, bool enable) {
  unsigned guid = system().port[port].device[device].input[input].guid;
  return inputManager->inputMap[guid]->rumble(enable);
}

unsigned Interface::dipSettings(const Markup::Node& node) {
  return dipSwitches->run(node);
}

string Interface::path(unsigned group) {
  return utility->path(group);
}

string Interface::server() {
  return {
    config->server.username, ":",
    config->server.password, "@",
    config->server.hostname
  };
}

void Interface::notify(string text) {
  MessageWindow().setParent(*presentation).setText(text).information();
}
