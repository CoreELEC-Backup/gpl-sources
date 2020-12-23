#version 450
precision highp float;
precision highp int;

#include "common.h"
#include "primitive.h"

#ifdef MSAA
layout(set = 0, binding = 3, input_attachment_index = 0) uniform mediump subpassInputMS uFeedbackFramebuffer;
#else
layout(set = 0, binding = 3, input_attachment_index = 0) uniform mediump subpassInput uFeedbackFramebuffer;
#endif

const int BLEND_ADD = 0;
const int BLEND_AVG = 1;
const int BLEND_SUB = 2;
const int BLEND_ADD_QUARTER = 3;
layout(constant_id = 2) const int BLEND_MODE = BLEND_ADD;

void main()
{
#ifdef TEXTURED
	vec4 NNColor = sample_vram_atlas(clamp_coord(vUV));
	if (all(equal(NNColor, vec4(0.0))))
		discard;

	vec4 color = NNColor;

	vec3 shaded = color.rgb * vColor.rgb * (255.0 / 128.0);
	float blend_amt = NNColor.a;
#else
	vec3 shaded = vColor.rgb;
	const float blend_amt = 1.0;
#endif

#ifdef MSAA
	// Need to be render per-sample here.
	vec4 fbcolor = subpassLoad(uFeedbackFramebuffer, gl_SampleID);
#else
	vec4 fbcolor = subpassLoad(uFeedbackFramebuffer);
#endif

	if (fbcolor.a > 0.5)
		discard;

	vec3 blended;
	if (BLEND_MODE == BLEND_ADD)
		blended = mix(shaded, shaded + fbcolor.rgb, blend_amt);
	if (BLEND_MODE == BLEND_AVG)
		blended = mix(shaded, 0.5 * (clamp(shaded, 0.0, 1.0) + fbcolor.rgb), blend_amt);
	if (BLEND_MODE == BLEND_SUB)
		blended = mix(shaded, fbcolor.rgb - shaded, blend_amt);
	if (BLEND_MODE == BLEND_ADD_QUARTER)
		blended = mix(shaded, clamp(shaded, 0.0, 1.0) * 0.25 + fbcolor.rgb, blend_amt);

#ifdef TEXTURED
	FragColor = vec4(blended, NNColor.a + vColor.a);
#else
	FragColor = vec4(blended, vColor.a);
#endif

	// Get round down behavior instead of round-to-nearest.
	// This is required for various "fade" out effects.
	// However, don't accidentially round down if we are already rounded to avoid
	// unintended feedback effects.
	FragColor.rgb -= 0.49 / 255.0;

#if 0
#if defined(TEXTURED)
	if ((vParam.z & 0x100) != 0)
		FragColor.rgb += textureLod(uDitherLUT, gl_FragCoord.xy * 0.25, 0.0).xxx - 4.0 / 255.0;
#endif
	FragColor.rgb = quantize_bgr555(FragColor.rgb);
#endif
}
