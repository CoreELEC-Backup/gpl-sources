/* Hyper NeoGeo 64 - 3D bits */

// todo, use poly.c

#include "includes/hng64.h"



// Hardware calls these '3d buffers'
//   They're only read during the startup check of fatfurwa.  Z-buffer memory?  Front buffer, back buffer?
//   They're definitely mirrored in the startup test, according to ElSemi
//   30100000-3011ffff is framebuffer A0
//   30120000-3013ffff is framebuffer A1
//   30140000-3015ffff is ZBuffer A

READ32_MEMBER(hng64_state::hng64_3d_1_r)
{
	return m_3d_1[offset];
}

WRITE32_MEMBER(hng64_state::hng64_3d_1_w)
{
	COMBINE_DATA (&m_3d_1[offset]);
}

READ32_MEMBER(hng64_state::hng64_3d_2_r)
{
	return m_3d_2[offset];
}

WRITE32_MEMBER(hng64_state::hng64_3d_2_w)
{
	COMBINE_DATA (&m_3d_2[offset]);
}

// The 3d 'display list'
WRITE16_MEMBER(hng64_state::dl_w)
{
	COMBINE_DATA(&m_dl[offset]);
}




/* TODO: different param for both Samurai games, less FIFO to process? */
WRITE32_MEMBER(hng64_state::dl_upload_w)
{
	// this is written after the game uploads 16 packets, each 32 bytes long (2x 16 words?)
	// we're assuming it to be a 'send to 3d hardware' trigger.
	// this can be called multiple times per frame (at least 2, as long as it gets the expected interrupt / status flags)

	for(int packetStart=0;packetStart<0x200;packetStart+=32)
	{
		// Send it off to the 3d subsystem.
		hng64_command3d( &m_dl[packetStart/2] );
	}

	machine().scheduler().timer_set(m_maincpu->cycles_to_attotime(0x200*8), timer_expired_delegate(FUNC(hng64_state::hng64_3dfifo_processed),this));
}

TIMER_CALLBACK_MEMBER(hng64_state::hng64_3dfifo_processed )
{
// ...
	m_set_irq(0x0008);
}


/* Note: Samurai Shodown games never calls bit 1, so it can't be framebuffer clear. It also calls bit 3 at start-up, meaning unknown */
WRITE32_MEMBER(hng64_state::dl_control_w) // This handles framebuffers
{
//	printf("dl_control_w %08x %08x\n", data, mem_mask);

	//if(data & 2) // swap buffers
	//{
	//  clear3d();
	//}

//  printf("%02x\n",data);

//  if(data & 1) // process DMA from 3d FIFO to framebuffer

//  if(data & 4) // reset buffer count
}




////////////////////
// 3d 'Functions' //
////////////////////

void hng64_state::printPacket(const UINT16* packet, int hex)
{
	if (hex)
	{
		printf("Packet : %04x %04x  2:%04x %04x  4:%04x %04x  6:%04x %04x  8:%04x %04x  10:%04x %04x  12:%04x %04x  14:%04x %04x\n",
				packet[0],  packet[1],
				packet[2],  packet[3],
				packet[4],  packet[5],
				packet[6],  packet[7],
				packet[8],  packet[9],
				packet[10], packet[11],
				packet[12], packet[13],
				packet[14], packet[15]);
	}
	else
	{
		printf("Packet : %04x %3.4f  2:%3.4f %3.4f  4:%3.4f %3.4f  6:%3.4f %3.4f  8:%3.4f %3.4f  10:%3.4f %3.4f  12:%3.4f %3.4f  14:%3.4f %3.4f\n",
				packet[0],            uToF(packet[1] )*128,
				uToF(packet[2] )*128, uToF(packet[3] )*128,
				uToF(packet[4] )*128, uToF(packet[5] )*128,
				uToF(packet[6] )*128, uToF(packet[7] )*128,
				uToF(packet[8] )*128, uToF(packet[9] )*128,
				uToF(packet[10])*128, uToF(packet[11])*128,
				uToF(packet[12])*128, uToF(packet[13])*128,
				uToF(packet[14])*128, uToF(packet[15])*128);
	}
}

// Operation 0001
// Camera transformation.
void hng64_state::setCameraTransformation(const UINT16* packet)
{
	float *cameraMatrix = m_cameraMatrix;

	/*//////////////
	// PACKET FORMAT
	// [0]  - 0001 ... ID
	// [1]  - xxxx ... Extrinsic camera matrix
	// [2]  - xxxx ... Extrinsic camera matrix
	// [3]  - xxxx ... Extrinsic camera matrix
	// [4]  - xxxx ... Extrinsic camera matrix
	// [5]  - xxxx ... Extrinsic camera matrix
	// [6]  - xxxx ... Extrinsic camera matrix
	// [7]  - xxxx ... Extrinsic camera matrix
	// [8]  - xxxx ... Extrinsic camera matrix
	// [9]  - xxxx ... Extrinsic camera matrix
	// [10] - xxxx ... Extrinsic camera matrix
	// [11] - xxxx ... Extrinsic camera matrix
	// [12] - xxxx ... Extrinsic camera matrix
	// [13] - ???? ... ? Flips per-frame during fatfurwa 'HNG64'
	// [14] - ???? ... ? Could be some floating-point values during buriki 'door run'
	// [15] - ???? ... ? Same as 13 & 14
	////////////*/
	// CAMERA TRANSFORMATION MATRIX
	cameraMatrix[0]  = uToF(packet[1]);
	cameraMatrix[4]  = uToF(packet[2]);
	cameraMatrix[8]  = uToF(packet[3]);
	cameraMatrix[3]  = 0.0f;

	cameraMatrix[1]  = uToF(packet[4]);
	cameraMatrix[5]  = uToF(packet[5]);
	cameraMatrix[9]  = uToF(packet[6]);
	cameraMatrix[7]  = 0.0f;

	cameraMatrix[2]  = uToF(packet[7]);
	cameraMatrix[6]  = uToF(packet[8]);
	cameraMatrix[10] = uToF(packet[9]);
	cameraMatrix[11] = 0.0f;

	cameraMatrix[12] = uToF(packet[10]);
	cameraMatrix[13] = uToF(packet[11]);
	cameraMatrix[14] = uToF(packet[12]);
	cameraMatrix[15] = 1.0f;
}

// Operation 0010
// Lighting information
void hng64_state::setLighting(const UINT16* packet)
{
	float *lightVector = m_lightVector;

	/*//////////////
	// PACKET FORMAT
	// [0]  - 0010 ... ID
	// [1]  - ???? ... ? Always zero
	// [2]  - ???? ... ? Always zero
	// [3]  - xxxx ... X light vector direction
	// [4]  - xxxx ... Y light vector direction
	// [5]  - xxxx ... Z light vector direction
	// [6]  - ???? ... ? Seems to be another light vector ?
	// [7]  - ???? ... ? Seems to be another light vector ?
	// [8]  - ???? ... ? Seems to be another light vector ?
	// [9]  - xxxx ... Strength according to sams64_2 [0000,01ff]
	// [10] - ???? ... ? Used in fatfurwa
	// [11] - ???? ... ? Used in fatfurwa
	// [12] - ???? ... ? Used in fatfurwa
	// [13] - ???? ... ? Used in fatfurwa
	// [14] - ???? ... ? Used in fatfurwa
	// [15] - ???? ... ? Used in fatfurwa
	////////////*/
	if (packet[1] != 0x0000) printf("ZOMG!  packet[1] in setLighting function is non-zero!\n");
	if (packet[2] != 0x0000) printf("ZOMG!  packet[2] in setLighting function is non-zero!\n");

	lightVector[0] = uToF(packet[3]);
	lightVector[1] = uToF(packet[4]);
	lightVector[2] = uToF(packet[5]);
	m_lightStrength = uToF(packet[9]);
}

// Operation 0011
// Palette / Model flags?
void hng64_state::set3dFlags(const UINT16* packet)
{
	/*//////////////
	// PACKET FORMAT
	// [0]  - 0011 ... ID
	// [1]  - ???? ...
	// [2]  - ???? ...
	// [3]  - ???? ...
	// [4]  - ???? ...
	// [5]  - ???? ...
	// [6]  - ???? ...
	// [7]  - ???? ...
	// [8]  - xx?? ... Palette offset & ??
	// [9]  - ???? ... ? Very much used - seem to bounce around when characters are on screen
	// [10] - ???? ... ? ''  ''
	// [11] - ???? ... ? ''  ''
	// [12] - ???? ... ? ''  ''
	// [13] - ???? ... ? ''  ''
	// [14] - ???? ... ? ''  ''
	// [15] - ???? ... ? ''  ''
	////////////*/
	m_paletteState3d = (packet[8] & 0xff00) >> 8;
}

// Operation 0012
// Projection Matrix.
void hng64_state::setCameraProjectionMatrix(const UINT16* packet)
{
	float *projectionMatrix = m_projectionMatrix;

	/*//////////////
	// PACKET FORMAT
	// [0]  - 0012 ... ID
	// [1]  - ???? ... ? Contains a value in buriki's 'how to play' - probably a projection window/offset.
	// [2]  - ???? ... ? Contains a value in buriki's 'how to play' - probably a projection window/offset.
	// [3]  - ???? ... ? Contains a value
	// [4]  - xxxx ... Camera projection near scale
	// [5]  - xxxx ... Camera projection near height(?)
	// [6]  - xxxx ... Camera projection near width(?)
	// [7]  - xxxx ... Camera projection far scale
	// [8]  - xxxx ... Camera projection far height(?)
	// [9]  - xxxx ... Camera projection far width(?)
	// [10] - xxxx ... Camera projection right
	// [11] - xxxx ... Camera projection left
	// [12] - xxxx ... Camera projection top
	// [13] - xxxx ... Camera projection bottom
	// [14] - ???? ... ? Gets data during buriki door-run
	// [15] - ???? ... ? Gets data during buriki door-run
	////////////*/

	// Heisted from GLFrustum - 6 parameters...
	float left, right, top, bottom, near_, far_;

	left    = uToF(packet[11]);
	right   = uToF(packet[10]);
	top     = uToF(packet[12]);
	bottom  = uToF(packet[13]);
	near_   = uToF(packet[6]) + (uToF(packet[6]) * uToF(packet[4]));
	far_    = uToF(packet[9]) + (uToF(packet[9]) * uToF(packet[7]));
	// (note are likely not 100% correct - I'm not using one of the parameters)

	projectionMatrix[0]  = (2.0f*near_)/(right-left);
	projectionMatrix[1]  = 0.0f;
	projectionMatrix[2]  = 0.0f;
	projectionMatrix[3]  = 0.0f;

	projectionMatrix[4]  = 0.0f;
	projectionMatrix[5]  = (2.0f*near_)/(top-bottom);
	projectionMatrix[6]  = 0.0f;
	projectionMatrix[7]  = 0.0f;

	projectionMatrix[8]  = (right+left)/(right-left);
	projectionMatrix[9]  = (top+bottom)/(top-bottom);
	projectionMatrix[10] = -((far_+near_)/(far_-near_));
	projectionMatrix[11] = -1.0f;

	projectionMatrix[12] = 0.0f;
	projectionMatrix[13] = 0.0f;
	projectionMatrix[14] = -((2.0f*far_*near_)/(far_-near_));
	projectionMatrix[15] = 0.0f;
}

// Operation 0100
// Polygon rasterization.
void hng64_state::recoverPolygonBlock(const UINT16* packet, struct polygon* polys, int* numPolys)
{
	/*//////////////
	// PACKET FORMAT
	// [0]  - 0100 ... ID
	// [1]  - ?--- ... Flags [?000 = ???
	//                        0?00 = ???
	//                        00?0 = ???
	//                        000? = ???]
	// [1]  - -?-- ... Flags [?000 = ???
	//                        0?00 = ???
	//                        00?0 = ???
	//                        000x = Dynamic palette bit]
	// [1]  - --?- ... Flags [?000 = ???
	//                        0?00 = ???
	//                        00?0 = ???
	//                        000? = ???]
	// [1]  - ---? ... Flags [x000 = Apply lighting bit
	//                        0?00 = ???
	//                        00?0 = ???
	//                        000? = ???]
	// [2]  - xxxx ... offset into ROM
	// [3]  - xxxx ... offset into ROM
	// [4]  - xxxx ... Transformation matrix
	// [5]  - xxxx ... Transformation matrix
	// [6]  - xxxx ... Transformation matrix
	// [7]  - xxxx ... Transformation matrix
	// [8]  - xxxx ... Transformation matrix
	// [9]  - xxxx ... Transformation matrix
	// [10] - xxxx ... Transformation matrix
	// [11] - xxxx ... Transformation matrix
	// [12] - xxxx ... Transformation matrix
	// [13] - xxxx ... Transformation matrix
	// [14] - xxxx ... Transformation matrix
	// [15] - xxxx ... Transformation matrix
	////////////*/



	float objectMatrix[16];
	setIdentity(objectMatrix);
	/////////////////
	// HEADER INFO //
	/////////////////
	// THE OBJECT TRANSFORMATION MATRIX
	objectMatrix[8] = uToF(packet[7]);
	objectMatrix[4] = uToF(packet[8]);
	objectMatrix[0] = uToF(packet[9]);
	objectMatrix[3] = 0.0f;

	objectMatrix[9] = uToF(packet[10]);
	objectMatrix[5] = uToF(packet[11]);
	objectMatrix[1] = uToF(packet[12]);
	objectMatrix[7] = 0.0f;

	objectMatrix[10] = uToF(packet[13]);
	objectMatrix[6 ] = uToF(packet[14]);
	objectMatrix[2 ] = uToF(packet[15]);
	objectMatrix[11] = 0.0f;

	objectMatrix[12] = uToF(packet[4]);
	objectMatrix[13] = uToF(packet[5]);
	objectMatrix[14] = uToF(packet[6]);
	objectMatrix[15] = 1.0f;

	UINT32 size[4];
	UINT32 address[4];
	UINT32 megaOffset;
	float eyeCoords[4];     // ObjectCoords transformed by the modelViewMatrix
//  float clipCoords[4];    // EyeCoords transformed by the projectionMatrix
	float ndCoords[4];      // Normalized device coordinates/clipCoordinates (x/w, y/w, z/w)
	float windowCoords[4];  // Mapped ndCoordinates to screen space
	float cullRay[4];
	struct polygon lastPoly = { 0 };
	const rectangle &visarea = m_screen->visible_area();


	//////////////////////////////////////////////////////////
	// EXTRACT DATA FROM THE ADDRESS POINTED TO IN THE FILE //
	//////////////////////////////////////////////////////////
	/*//////////////////////////////////////////////
	// DIRECTLY-POINTED-TO FORMAT (7 words x 3 ROMs)
	// [0]  - lower word of sub-address 1
	// [1]  - lower word of sub-address 2
	// [2]  - upper word of all sub-addresses
	// [3]  - lower word of sub-address 3
	// [4]  - lower word of sub-address 4
	// [5]  - ???? always 0 ????
	// [6]  - number of chunks in sub-address 1 block
	// [7]  - number of chunks in sub-address 2 block
	// [8]  - ???? always 0 ????
	// [9]  - number of chunks in sub-address 3 block
	// [10] - number of chunks in sub-address 4 block
	// [11] - ? definitely used.
	// [12] - ? definitely used.
	// [13] - ? definitely used.
	// [14] - ? definitely used.
	// [15] - ???? always 0 ????
	// [16] - ???? always 0 ????
	// [17] - ???? always 0 ????
	// [18] - ???? always 0 ????
	// [19] - ???? always 0 ????
	// [20] - ???? always 0 ????
	//////////////////////////////////////////////*/

	// 3d ROM Offset
	UINT16* threeDRoms = m_vertsrom;
	UINT32  threeDOffset = (((UINT32)packet[2]) << 16) | ((UINT32)packet[3]);
	UINT16* threeDPointer = &threeDRoms[threeDOffset * 3];

	if (threeDOffset >= m_vertsrom_size)
	{
		printf("Strange geometry packet: (ignoring)\n");
		printPacket(packet, 1);
		return;
	}

#if 0
	// Debug - ajg
	printf("%08x : ", threeDOffset*3*2);
	for (int k = 0; k < 7*3; k++)
	{
		printf("%04x ", threeDPointer[k]);
		if ((k % 3) == 2) printf(" ");
	}
	printf("\n");
#endif

	// There are 4 hunks per address.
	address[0] = threeDPointer[0];
	address[1] = threeDPointer[1];
	megaOffset = threeDPointer[2];

	address[2] = threeDPointer[3];
	address[3] = threeDPointer[4];
	if (threeDPointer[5] != 0x0000) printf("ZOMG!  3dPointer[5] is non-zero!\n");

	size[0]    = threeDPointer[6];
	size[1]    = threeDPointer[7];
	if (threeDPointer[8] != 0x0000) printf("ZOMG!  3dPointer[8] is non-zero!\n");

	size[2]    = threeDPointer[9];
	size[3]    = threeDPointer[10];
	/*           ????         [11]; Used. */

	/*           ????         [12]; Used. */
	/*           ????         [13]; Used. */
	/*           ????         [14]; Used. */

	if (threeDPointer[15] != 0x0000) printf("ZOMG!  3dPointer[15] is non-zero!\n");
	if (threeDPointer[16] != 0x0000) printf("ZOMG!  3dPointer[16] is non-zero!\n");
	if (threeDPointer[17] != 0x0000) printf("ZOMG!  3dPointer[17] is non-zero!\n");

	if (threeDPointer[18] != 0x0000) printf("ZOMG!  3dPointer[18] is non-zero!\n");
	if (threeDPointer[19] != 0x0000) printf("ZOMG!  3dPointer[19] is non-zero!\n");
	if (threeDPointer[20] != 0x0000) printf("ZOMG!  3dPointer[20] is non-zero!\n");

	/* Concatenate the megaOffset with the addresses */
	address[0] |= (megaOffset << 16);
	address[1] |= (megaOffset << 16);
	address[2] |= (megaOffset << 16);
	address[3] |= (megaOffset << 16);

	// Debug - ajg
	//UINT32 tdColor = 0xff000000;
	//if (threeDPointer[14] & 0x0002) tdColor |= 0x00ff0000;
	//if (threeDPointer[14] & 0x0001) tdColor |= 0x0000ff00;
	//if (threeDPointer[14] & 0x0000) tdColor |= 0x000000ff;

	/* For all 4 polygon chunks */
	for (int k = 0; k < 4; k++)
	{
		UINT16* chunkOffset = &threeDRoms[address[k] * 3];
		for (int l = 0; l < size[k]; l++)
		{
			////////////////////////////////////////////
			// GATHER A SINGLE TRIANGLE'S INFORMATION //
			////////////////////////////////////////////
			// SINGLE POLY CHUNK FORMAT
			////////////////////////////////////////////
			// GATHER A SINGLE TRIANGLE'S INFORMATION //
			////////////////////////////////////////////
			// SINGLE POLY CHUNK FORMAT
			// [0] 0000 0000 cccc cccc    0 = always 0 | c = chunk type / format of data that follows (see below)
			// [1] u--l pppp pppp ssss    u = unknown, always on for most games, on for the backgrounds only on sams64, l = low-res texture?  p = palette?  s = texture sheet (1024 x 1024 pages)
			// [2] S?XX *--- -YY# ----    S = use 4x4 sub-texture pages?  ? = SNK logo roadedge / bbust2 / broken banners in xrally,  XX = horizontal subtexture  * = broken banners in xrally  YY = vertical subtexture  @ = broken banners in xrally

			// we currently use one of the palette bits to enable a different palette mode.. seems hacky...
			// looks like vertical / horizontal sub-pages might be 3 bits, not 2,  ? could be enable bit for that..

			// 'Welcome to South Africa' roadside banner on xrally | 000e 8c0d d870 or 0096 8c0d d870  (8c0d, d870 seems key 1000 1100 0000 1101
			//                                                                                                               1101 1000 0111 0000 )


			UINT8 chunkType = chunkOffset[0] & 0x00ff;

			// Debug - ajg
			if (chunkOffset[0] & 0xff00)
			{
				printf("Weird!  The top byte of the chunkType has a value %04x!\n", chunkOffset[0]);
				continue;
			}

			// Debug - Colors polygons with certain flags bright blue! ajg
			polys[*numPolys].debugColor = 0;
			//polys[*numPolys].debugColor = tdColor;

			// Debug - ajg
			//printf("%d (%08x) : %04x %04x %04x\n", k, address[k]*3*2, chunkOffset[0], chunkOffset[1], chunkOffset[2]);
			//break;

			// TEXTURE
			/* There may be more than just high & low res texture types, so I'm keeping texType as a UINT8. */
			if (chunkOffset[1] & 0x1000) polys[*numPolys].texType = 0x1;
			else                         polys[*numPolys].texType = 0x0;

			polys[*numPolys].texPageSmall       = (chunkOffset[2] & 0xc000)>>14;  // Just a guess.
			polys[*numPolys].texPageHorizOffset = (chunkOffset[2] & 0x3800) >> 11;
			polys[*numPolys].texPageVertOffset  = (chunkOffset[2] & 0x0070) >> 4;

			polys[*numPolys].texIndex = chunkOffset[1] & 0x000f;


			// PALETTE
			polys[*numPolys].palOffset = 0;
			polys[*numPolys].palPageSize = 0x100;

			/* FIXME: This isn't correct.
			          Buriki & Xrally need this line.  Roads Edge needs it removed.
			          So instead we're looking for a bit that is on for XRally & Buriki, but noone else. */
			if (m_3dregs[0x00/4] & 0x2000)
			{
				if (strcmp(machine().basename(), "roadedge"))
					polys[*numPolys].palOffset += 0x800;
			}

			//UINT16 explicitPaletteValue0 = ((chunkOffset[?] & 0x????) >> ?) * 0x800;
			UINT16 explicitPaletteValue1 = ((chunkOffset[1] & 0x0f00) >> 8) * 0x080;
			UINT16 explicitPaletteValue2 = ((chunkOffset[1] & 0x00f0) >> 4) * 0x008;

			// The presence of 0x00f0 *probably* sets 0x10-sized palette addressing.
			if (explicitPaletteValue2) polys[*numPolys].palPageSize = 0x10;

			// Apply the dynamic palette offset if its flag is set, otherwise stick with the fixed one
			if ((packet[1] & 0x0100))
			{
				explicitPaletteValue1 = m_paletteState3d * 0x80;
				explicitPaletteValue2 = 0;      // This is probably hiding somewhere in operation 0011
			}

			polys[*numPolys].palOffset += (explicitPaletteValue1 + explicitPaletteValue2);


#if 0
			if (((chunkOffset[2] & 0xc000) == 0x4000) && (m_screen->frame_number() & 1))
			{
			//	if (chunkOffset[2] == 0xd870)
				{
					polys[*numPolys].debugColor = 0xffff0000;
					printf("%d (%08x) : %04x %04x %04x\n", k, address[k] * 3 * 2, chunkOffset[0], chunkOffset[1], chunkOffset[2]);
				}
			}
#endif

			UINT8 chunkLength = 0;
			switch(chunkType)
			{
			/*/////////////////////////
			// CHUNK TYPE BITS - These are very likely incorrect.
			// x--- ---- - 1 = Has only 1 vertex (part of a triangle fan/strip)
			// -x-- ---- -
			// --x- ---- -
			// ---x ---- -
			// ---- x--- -
			// ---- -x-- - 1 = Has per-vert UVs
			// ---- --x- -
			// ---- ---x - 1 = Has per-vert normals
			/////////////////////////*/

			// 33 word chunk, 3 vertices, per-vertex UVs & normals, per-face normal
			case 0x05:  // 0000 0101
			case 0x0f:  // 0000 1111
				for (int m = 0; m < 3; m++)
				{
					polys[*numPolys].vert[m].worldCoords[0] = uToF(chunkOffset[3 + (9*m)]);
					polys[*numPolys].vert[m].worldCoords[1] = uToF(chunkOffset[4 + (9*m)]);
					polys[*numPolys].vert[m].worldCoords[2] = uToF(chunkOffset[5 + (9*m)]);
					polys[*numPolys].vert[m].worldCoords[3] = 1.0f;
					polys[*numPolys].n = 3;

					// chunkOffset[6 + (9*m)] is almost always 0080, but it's 0070 for the translucent globe in fatfurwa player select
					polys[*numPolys].vert[m].texCoords[0] = uToF(chunkOffset[7 + (9*m)]);
					polys[*numPolys].vert[m].texCoords[1] = uToF(chunkOffset[8 + (9*m)]);
					polys[*numPolys].vert[m].texCoords[2] = 0.0f;
					polys[*numPolys].vert[m].texCoords[3] = 1.0f;

					polys[*numPolys].vert[m].normal[0] = uToF(chunkOffset[9  + (9*m)]);
					polys[*numPolys].vert[m].normal[1] = uToF(chunkOffset[10 + (9*m)] );
					polys[*numPolys].vert[m].normal[2] = uToF(chunkOffset[11 + (9*m)] );
					polys[*numPolys].vert[m].normal[3] = 0.0f;
				}

				// Redundantly called, but it works...
				polys[*numPolys].faceNormal[0] = uToF(chunkOffset[30]);
				polys[*numPolys].faceNormal[1] = uToF(chunkOffset[31]);
				polys[*numPolys].faceNormal[2] = uToF(chunkOffset[32]);
				polys[*numPolys].faceNormal[3] = 0.0f;

				chunkLength = 33;
				break;


			// 24 word chunk, 3 vertices, per-vertex UVs
			case 0x04:  // 0000 0100
			case 0x0e:  // 0000 1110
			case 0x24:  // 0010 0100
			case 0x2e:  // 0010 1110
				for (int m = 0; m < 3; m++)
				{
					polys[*numPolys].vert[m].worldCoords[0] = uToF(chunkOffset[3 + (6*m)]);
					polys[*numPolys].vert[m].worldCoords[1] = uToF(chunkOffset[4 + (6*m)]);
					polys[*numPolys].vert[m].worldCoords[2] = uToF(chunkOffset[5 + (6*m)]);
					polys[*numPolys].vert[m].worldCoords[3] = 1.0f;
					polys[*numPolys].n = 3;

					// chunkOffset[6 + (6*m)] is almost always 0080, but it's 0070 for the translucent globe in fatfurwa player select
					polys[*numPolys].vert[m].texCoords[0] = uToF(chunkOffset[7 + (6*m)]);
					polys[*numPolys].vert[m].texCoords[1] = uToF(chunkOffset[8 + (6*m)]);
					polys[*numPolys].vert[m].texCoords[2] = 0.0f;
					polys[*numPolys].vert[m].texCoords[3] = 1.0f;

					polys[*numPolys].vert[m].normal[0] = uToF(chunkOffset[21]);
					polys[*numPolys].vert[m].normal[1] = uToF(chunkOffset[22]);
					polys[*numPolys].vert[m].normal[2] = uToF(chunkOffset[23]);
					polys[*numPolys].vert[m].normal[3] = 0.0f;
				}

				// Redundantly called, but it works...
				polys[*numPolys].faceNormal[0] = polys[*numPolys].vert[2].normal[0];
				polys[*numPolys].faceNormal[1] = polys[*numPolys].vert[2].normal[1];
				polys[*numPolys].faceNormal[2] = polys[*numPolys].vert[2].normal[2];
				polys[*numPolys].faceNormal[3] = 0.0f;

				chunkLength = 24;
				break;


			// 15 word chunk, 1 vertex, per-vertex UVs & normals, face normal
			case 0x87:  // 1000 0111
			case 0x97:  // 1001 0111
			case 0xd7:  // 1101 0111
			case 0xc7:  // 1100 0111
				// Copy over the proper vertices from the previous triangle...
				memcpy(&polys[*numPolys].vert[1], &lastPoly.vert[0], sizeof(struct polyVert));
				memcpy(&polys[*numPolys].vert[2], &lastPoly.vert[2], sizeof(struct polyVert));

				// Fill in the appropriate data...
				polys[*numPolys].vert[0].worldCoords[0] = uToF(chunkOffset[3]);
				polys[*numPolys].vert[0].worldCoords[1] = uToF(chunkOffset[4]);
				polys[*numPolys].vert[0].worldCoords[2] = uToF(chunkOffset[5]);
				polys[*numPolys].vert[0].worldCoords[3] = 1.0f;
				polys[*numPolys].n = 3;

				// chunkOffset[6] is almost always 0080, but it's 0070 for the translucent globe in fatfurwa player select
				polys[*numPolys].vert[0].texCoords[0] = uToF(chunkOffset[7]);
				polys[*numPolys].vert[0].texCoords[1] = uToF(chunkOffset[8]);
				polys[*numPolys].vert[0].texCoords[2] = 0.0f;
				polys[*numPolys].vert[0].texCoords[3] = 1.0f;

				polys[*numPolys].vert[0].normal[0] = uToF(chunkOffset[9]);
				polys[*numPolys].vert[0].normal[1] = uToF(chunkOffset[10]);
				polys[*numPolys].vert[0].normal[2] = uToF(chunkOffset[11]);
				polys[*numPolys].vert[0].normal[3] = 0.0f;

				polys[*numPolys].faceNormal[0] = uToF(chunkOffset[12]);
				polys[*numPolys].faceNormal[1] = uToF(chunkOffset[13]);
				polys[*numPolys].faceNormal[2] = uToF(chunkOffset[14]);
				polys[*numPolys].faceNormal[3] = 0.0f;

				chunkLength = 15;
				break;


			// 12 word chunk, 1 vertex, per-vertex UVs
			case 0x86:  // 1000 0110
			case 0x96:  // 1001 0110
			case 0xb6:  // 1011 0110
			case 0xc6:  // 1100 0110
			case 0xd6:  // 1101 0110
				// Copy over the proper vertices from the previous triangle...
				memcpy(&polys[*numPolys].vert[1], &lastPoly.vert[0], sizeof(struct polyVert));
				memcpy(&polys[*numPolys].vert[2], &lastPoly.vert[2], sizeof(struct polyVert));

				polys[*numPolys].vert[0].worldCoords[0] = uToF(chunkOffset[3]);
				polys[*numPolys].vert[0].worldCoords[1] = uToF(chunkOffset[4]);
				polys[*numPolys].vert[0].worldCoords[2] = uToF(chunkOffset[5]);
				polys[*numPolys].vert[0].worldCoords[3] = 1.0f;
				polys[*numPolys].n = 3;

				// chunkOffset[6] is almost always 0080, but it's 0070 for the translucent globe in fatfurwa player select
				polys[*numPolys].vert[0].texCoords[0] = uToF(chunkOffset[7]);
				polys[*numPolys].vert[0].texCoords[1] = uToF(chunkOffset[8]);
				polys[*numPolys].vert[0].texCoords[2] = 0.0f;
				polys[*numPolys].vert[0].texCoords[3] = 1.0f;

				// This normal could be right, but I'm not entirely sure - there is no normal in the 18 bytes!
				polys[*numPolys].vert[0].normal[0] = lastPoly.faceNormal[0];
				polys[*numPolys].vert[0].normal[1] = lastPoly.faceNormal[1];
				polys[*numPolys].vert[0].normal[2] = lastPoly.faceNormal[2];
				polys[*numPolys].vert[0].normal[3] = lastPoly.faceNormal[3];

				polys[*numPolys].faceNormal[0] = lastPoly.faceNormal[0];
				polys[*numPolys].faceNormal[1] = lastPoly.faceNormal[1];
				polys[*numPolys].faceNormal[2] = lastPoly.faceNormal[2];
				polys[*numPolys].faceNormal[3] = lastPoly.faceNormal[3];

				// TODO: I'm not reading 3 necessary words here (maybe face normal) !!!

#if 0
				// DEBUG
				printf("0x?6 : %08x (%d/%d)\n", address[k]*3*2, l, size[k]-1);
				for (int m = 0; m < 13; m++)
					printf("%04x ", chunkOffset[m]);
				printf("\n");

				for (int m = 0; m < 13; m++)
					printf("%3.4f ", uToF(chunkOffset[m]));
				printf("\n\n");
#endif

				chunkLength = 12;
				break;

			default:
				printf("UNKNOWN geometry CHUNK TYPE : %02x\n", chunkType);
				chunkLength = 0;
				break;
			}

			polys[*numPolys].visible = 1;

			// Backup the last polygon (for triangle fans [strips?])
			memcpy(&lastPoly, &polys[*numPolys], sizeof(struct polygon));


			////////////////////////////////////
			// Project and clip               //
			////////////////////////////////////
			// Perform the world transformations...
			// !! Can eliminate this step with a matrix stack (maybe necessary?) !!
			setIdentity(m_modelViewMatrix);
			if (m_mcu_type != SAMSHO_MCU)
			{
				// The sams64 games transform the geometry in front of a stationary camera.
				// This is fine in sams64_2, since it never calls the 'camera transformation' function
				// (thus using the identity matrix for this transform), but sams64 calls the
				// camera transformation function with rotation values.
				// It remains to be seen what those might do...
				matmul4(m_modelViewMatrix, m_modelViewMatrix, m_cameraMatrix);
			}
			matmul4(m_modelViewMatrix, m_modelViewMatrix, objectMatrix);

			// LIGHTING
			if (packet[1] & 0x0008 && m_lightStrength > 0.0f)
			{
				for (int v = 0; v < 3; v++)
				{
					float transformedNormal[4];
					vecmatmul4(transformedNormal, objectMatrix, polys[*numPolys].vert[v].normal);
					normalize(transformedNormal);
					normalize(m_lightVector);

					float intensity = vecDotProduct(transformedNormal, m_lightVector) * -1.0f;
					intensity = (intensity <= 0.0f) ? (0.0f) : (intensity);
					intensity *= m_lightStrength * 128.0f;    // Turns 0x0100 into 1.0
					intensity *= 128.0;                     // Maps intensity to the range [0.0, 2.0]
					if (intensity >= 255.0f) intensity = 255.0f;

					polys[*numPolys].vert[v].light[0] = intensity;
					polys[*numPolys].vert[v].light[1] = intensity;
					polys[*numPolys].vert[v].light[2] = intensity;
				}
			}
			else
			{
				// Just clear out the light values
				for (int v = 0; v < 3; v++)
				{
					polys[*numPolys].vert[v].light[0] = 0;
					polys[*numPolys].vert[v].light[1] = 0;
					polys[*numPolys].vert[v].light[2] = 0;
				}
			}


			// BACKFACE CULL //
			// EMPIRICAL EVIDENCE SEEMS TO SHOW THE HNG64 HARDWARE DOES NOT BACKFACE CULL //
#if 0
			float cullRay[4];
			float cullNorm[4];

			// Cast a ray out of the camera towards the polygon's point in eyespace.
			vecmatmul4(cullRay, modelViewMatrix, polys[*numPolys].vert[0].worldCoords);
			normalize(cullRay);
			// Dot product that with the normal to see if you're negative...
			vecmatmul4(cullNorm, modelViewMatrix, polys[*numPolys].faceNormal);

			float result = vecDotProduct(cullRay, cullNorm);

			if (result < 0.0f)
				polys[*numPolys].visible = 1;
			else
				polys[*numPolys].visible = 0;
#endif


			// BEHIND-THE-CAMERA CULL //
			vecmatmul4(cullRay, m_modelViewMatrix, polys[*numPolys].vert[0].worldCoords);
			if (cullRay[2] > 0.0f)              // Camera is pointing down -Z
			{
				polys[*numPolys].visible = 0;
			}


			// TRANSFORM THE TRIANGLE INTO HOMOGENEOUS SCREEN SPACE //
			if (polys[*numPolys].visible)
			{
				for (int m = 0; m < polys[*numPolys].n; m++)
				{
					// Transform and project the vertex into pre-divided homogeneous coordinates...
					vecmatmul4(eyeCoords, m_modelViewMatrix, polys[*numPolys].vert[m].worldCoords);
					vecmatmul4(polys[*numPolys].vert[m].clipCoords, m_projectionMatrix, eyeCoords);
				}

				if (polys[*numPolys].visible)
				{
					// Clip the triangles to the view frustum...
					performFrustumClip(&polys[*numPolys]);

					for (int m = 0; m < polys[*numPolys].n; m++)
					{
						// Convert into normalized device coordinates...
						ndCoords[0] = polys[*numPolys].vert[m].clipCoords[0] / polys[*numPolys].vert[m].clipCoords[3];
						ndCoords[1] = polys[*numPolys].vert[m].clipCoords[1] / polys[*numPolys].vert[m].clipCoords[3];
						ndCoords[2] = polys[*numPolys].vert[m].clipCoords[2] / polys[*numPolys].vert[m].clipCoords[3];
						ndCoords[3] = polys[*numPolys].vert[m].clipCoords[3];

						// Final pixel values are garnered here :
						windowCoords[0] = (ndCoords[0]+1.0f) * ((float)(visarea.max_x) / 2.0f) + 0.0f;
						windowCoords[1] = (ndCoords[1]+1.0f) * ((float)(visarea.max_y) / 2.0f) + 0.0f;
						windowCoords[2] = (ndCoords[2]+1.0f) * 0.5f;

						windowCoords[1] = (float)visarea.max_y - windowCoords[1];       // Flip Y

						// Store the points in a list for later use...
						polys[*numPolys].vert[m].clipCoords[0] = windowCoords[0];
						polys[*numPolys].vert[m].clipCoords[1] = windowCoords[1];
						polys[*numPolys].vert[m].clipCoords[2] = windowCoords[2];
						polys[*numPolys].vert[m].clipCoords[3] = ndCoords[3];
					}
				}
			}

			// Advance to the next polygon chunk...
			chunkOffset += chunkLength;

			(*numPolys)++;
		}
	}
}

// note 0x0102 packets are only 8 words, it appears they can be in either the upper or lower half of the 16 word packet.
// We currently only draw 0x0102 packets where both halves contain 0x0102 (2 calls), but this causes graphics to vanish in xrally because in some cases the 0x0102 packet only exists in the upper or lower half
// with another value (often 0x0000 - NOP) in the other.
// If we also treat (0x0000 - NOP) as 8 word  instead of 16 so that we can access a 0x0102 in the 2nd half of the 16 word packet then we end up with other invalid packets in the 2nd half which should be ignored.
// This would suggest our processing if flawed in other ways, or there is something else to indicate packet length.

void hng64_state::hng64_command3d(const UINT16* packet)
{

	/* A temporary place to put some polygons.  This will optimize away if the compiler's any good. */
	int numPolys = 0;
	dynamic_array<polygon> polys(1024*5);

	//printf("packet type : %04x %04x|%04x %04x|%04x %04x|%04x %04x  | %04x %04x %04x %04x %04x %04x %04x %04x\n", packet[0],packet[1],packet[2],packet[3],packet[4],packet[5],packet[6],packet[7],     packet[8], packet[9], packet[10], packet[11], packet[12], packet[13], packet[14], packet[15]);
	
	switch (packet[0])
	{
	case 0x0000:    // Appears to be a NOP.
		break;

	case 0x0001:    // Camera transformation.
		setCameraTransformation(packet);
		break;

	case 0x0010:    // Lighting information.
		//if (packet[9]) printPacket(packet, 1);
		setLighting(packet);
		break;

	case 0x0011:    // Palette / Model flags?
		//printPacket(packet, 1); printf("\n");
		set3dFlags(packet);
		break;

	case 0x0012:    // Projection Matrix
		//printPacket(packet, 1);
		setCameraProjectionMatrix(packet);
		break;

	case 0x0100:
	case 0x0101:    // Geometry with full transformations
		// HACK.  Masks out a piece of geo bbust2's drawShaded() crashes on.
		if (packet[2] == 0x0003 && packet[3] == 0x8f37 && m_mcu_type == SHOOT_MCU)
			break;

		recoverPolygonBlock( packet, polys, &numPolys);
		break;

	case 0x0102:    // Geometry with only translation
		// HACK.  Give up on strange calls to 0102.
		if (packet[8] != 0x0102)
		{
			// It appears as though packet[7] might hold the magic #
			// Almost looks like there is a chain mode for these guys.  Same for 0101?
			// printf("WARNING: "); printPacket(packet, 1);
			break;
		}

		// Split the packet and call recoverPolygonBlock on each half.
		UINT16 miniPacket[16];
		memset(miniPacket, 0, sizeof(UINT16)*16);
		for (int i = 0; i < 7; i++) miniPacket[i] = packet[i];
		miniPacket[7] = 0x7fff;
		miniPacket[11] = 0x7fff;
		miniPacket[15] = 0x7fff;
		recoverPolygonBlock( miniPacket, polys, &numPolys);

		memset(miniPacket, 0, sizeof(UINT16)*16);
		for (int i = 0; i < 7; i++) miniPacket[i] = packet[i+8];
		for (int i = 0; i < 7; i++) miniPacket[i] = packet[i+8];
		miniPacket[7] = 0x7fff;
		miniPacket[11] = 0x7fff;
		miniPacket[15] = 0x7fff;
		recoverPolygonBlock( miniPacket, polys, &numPolys);
		break;

	case 0x1000:    // Unknown: Some sort of global flags?
		//printPacket(packet, 1); printf("\n");
		break;

	case 0x1001:    // Unknown: Some sort of global flags (a group of 4, actually)?
		//printPacket(packet, 1);
		break;

	default:
		printf("HNG64: Unknown 3d command %04x.\n", packet[0]);
		break;
	}

	/* If there are polygons, rasterize them into the display buffer */
	for (int i = 0; i < numPolys; i++)
	{
		if (polys[i].visible)
		{
			drawShaded( &polys[i]);
		}
	}
}

void hng64_state::clear3d()
{
	int i;

	const rectangle &visarea = m_screen->visible_area();

	// Reset the buffers...
	for (i = 0; i < (visarea.max_x)*(visarea.max_y); i++)
	{
		m_depthBuffer3d[i] = 100.0f;
		m_colorBuffer3d[i] = rgb_t(0, 0, 0, 0);
	}

	// Set some matrices to the identity...
	setIdentity(m_projectionMatrix);
	setIdentity(m_modelViewMatrix);
	setIdentity(m_cameraMatrix);
}

/* 3D/framebuffer video registers
 * ------------------------------
 *
 * UINT32 | Bits                                    | Use
 *        | 3322 2222 2222 1111 1111 11             |
 * -------+-1098-7654-3210-9876-5432-1098-7654-3210-+----------------
 *      0 | ---- --x- ---- ---- ---- ---- ---- ---- | Reads in Fatal Fury WA, if on then there isn't a 3d refresh (busy flag?).
 *      0 | ---- ---x ---- ---- ---- ---- ---- ---- | set at POST/service modes, almost likely fb disable
 *      0 | ???? ???? ???? ???? ccc? ???? ???? ???? | framebuffer color base, 0x311800 in Fatal Fury WA, 0x313800 in Buriki One
 *      1 |                                         |
 *      2 | ???? ???? ???? ???? ???? ???? ???? ???? | camera / framebuffer global x/y? Actively used by Samurai Shodown 64 2
 *      3 | ---- --?x ---- ---- ---- ---- ---- ---- | unknown, unsetted by Buriki One and setted by Fatal Fury WA, buffering mode?
 *   4-11 | ---- ???? ---- ???? ---- ???? ---- ???? | Table filled with 0x0? data
 *
 */

/////////////////////
// 3D UTILITY CODE //
/////////////////////

/* 4x4 matrix multiplication */
void hng64_state::matmul4(float *product, const float *a, const float *b )
{
	int i;
	for (i = 0; i < 4; i++)
	{
		const float ai0 = a[0  + i];
		const float ai1 = a[4  + i];
		const float ai2 = a[8  + i];
		const float ai3 = a[12 + i];

		product[0  + i] = ai0 * b[0 ] + ai1 * b[1 ] + ai2 * b[2 ] + ai3 * b[3 ];
		product[4  + i] = ai0 * b[4 ] + ai1 * b[5 ] + ai2 * b[6 ] + ai3 * b[7 ];
		product[8  + i] = ai0 * b[8 ] + ai1 * b[9 ] + ai2 * b[10] + ai3 * b[11];
		product[12 + i] = ai0 * b[12] + ai1 * b[13] + ai2 * b[14] + ai3 * b[15];
	}
}

/* vector by 4x4 matrix multiply */
void hng64_state::vecmatmul4(float *product, const float *a, const float *b)
{
	const float bi0 = b[0];
	const float bi1 = b[1];
	const float bi2 = b[2];
	const float bi3 = b[3];

	product[0] = bi0 * a[0] + bi1 * a[4] + bi2 * a[8 ] + bi3 * a[12];
	product[1] = bi0 * a[1] + bi1 * a[5] + bi2 * a[9 ] + bi3 * a[13];
	product[2] = bi0 * a[2] + bi1 * a[6] + bi2 * a[10] + bi3 * a[14];
	product[3] = bi0 * a[3] + bi1 * a[7] + bi2 * a[11] + bi3 * a[15];
}

float hng64_state::vecDotProduct(const float *a, const float *b)
{
	return ((a[0]*b[0]) + (a[1]*b[1]) + (a[2]*b[2]));
}

void hng64_state::setIdentity(float *matrix)
{
	int i;

	for (i = 0; i < 16; i++)
	{
		matrix[i] = 0.0f;
	}

	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
}

float hng64_state::uToF(UINT16 input)
{
	float retVal;
	retVal = (float)((INT16)input) / 32768.0f;
	return retVal;

#if 0
	if ((INT16)input < 0)
		retVal = (float)((INT16)input) / 32768.0f;
	else
		retVal = (float)((INT16)input) / 32767.0f;
#endif
}

void hng64_state::normalize(float* x)
{
	double l2 = (x[0]*x[0]) + (x[1]*x[1]) + (x[2]*x[2]);
	double l = sqrt(l2);

	x[0] = (float)(x[0] / l);
	x[1] = (float)(x[1] / l);
	x[2] = (float)(x[2] / l);
}



///////////////////////////
// POLYGON CLIPPING CODE //
///////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The remainder of the code in this file is heavily                             //
//   influenced by, and sometimes copied verbatim from Andrew Zaferakis' SoftGL  //
//   rasterizing system.                                                         //
//                                                                               //
//   Andrew granted permission for its use in MAME in October of 2004.           //
///////////////////////////////////////////////////////////////////////////////////



int hng64_state::Inside(struct polyVert *v, int plane)
{
	switch(plane)
	{
	case HNG64_LEFT:
		return (v->clipCoords[0] >= -v->clipCoords[3]) ? 1 : 0;
	case HNG64_RIGHT:
		return (v->clipCoords[0] <=  v->clipCoords[3]) ? 1 : 0;

	case HNG64_TOP:
		return (v->clipCoords[1] <=  v->clipCoords[3]) ? 1 : 0;
	case HNG64_BOTTOM:
		return (v->clipCoords[1] >= -v->clipCoords[3]) ? 1 : 0;

	case HNG64_NEAR:
		return (v->clipCoords[2] <=  v->clipCoords[3]) ? 1 : 0;
	case HNG64_FAR:
		return (v->clipCoords[2] >= -v->clipCoords[3]) ? 1 : 0;
	}

	return 0;
}

void hng64_state::Intersect(struct polyVert *input0, struct polyVert *input1, struct polyVert *output, int plane)
{
	float t = 0.0f;

	float *Iv0 = input0->clipCoords;
	float *Iv1 = input1->clipCoords;
	float *Ov  = output->clipCoords;

	float *It0 = input0->texCoords;
	float *It1 = input1->texCoords;
	float *Ot  = output->texCoords;

	float *Il0 = input0->light;
	float *Il1 = input1->light;
	float *Ol  = output->light;

	switch(plane)
	{
	case HNG64_LEFT:
		t = (Iv0[0]+Iv0[3]) / (-Iv1[3]+Iv0[3]-Iv1[0]+Iv0[0]);
		break;
	case HNG64_RIGHT:
		t = (Iv0[0]-Iv0[3]) / (Iv1[3]-Iv0[3]-Iv1[0]+Iv0[0]);
		break;
	case HNG64_TOP:
		t = (Iv0[1]-Iv0[3]) / (Iv1[3]-Iv0[3]-Iv1[1]+Iv0[1]);
		break;
	case HNG64_BOTTOM:
		t = (Iv0[1]+Iv0[3]) / (-Iv1[3]+Iv0[3]-Iv1[1]+Iv0[1]);
		break;
	case HNG64_NEAR:
		t = (Iv0[2]-Iv0[3]) / (Iv1[3]-Iv0[3]-Iv1[2]+Iv0[2]);
		break;
	case HNG64_FAR:
		t = (Iv0[2]+Iv0[3]) / (-Iv1[3]+Iv0[3]-Iv1[2]+Iv0[2]);
		break;
	}

	Ov[0] = Iv0[0] + (Iv1[0] - Iv0[0]) * t;
	Ov[1] = Iv0[1] + (Iv1[1] - Iv0[1]) * t;
	Ov[2] = Iv0[2] + (Iv1[2] - Iv0[2]) * t;
	Ov[3] = Iv0[3] + (Iv1[3] - Iv0[3]) * t;

	Ot[0] = It0[0] + (It1[0] - It0[0]) * t;
	Ot[1] = It0[1] + (It1[1] - It0[1]) * t;
	Ot[2] = It0[2] + (It1[2] - It0[2]) * t;
	Ot[3] = It0[3] + (It1[3] - It0[3]) * t;

	Ol[0] = Il0[0] + (Il1[0] - Il0[0]) * t;
	Ol[1] = Il0[1] + (Il1[1] - Il0[1]) * t;
	Ol[2] = Il0[2] + (Il1[2] - Il0[2]) * t;
}

void hng64_state::performFrustumClip(struct polygon *p)
{
	int i, j, k;
	//////////////////////////////////////////////////////////////////////////
	// Clip against the volumes defined by the homogeneous clip coordinates //
	//////////////////////////////////////////////////////////////////////////

	struct polygon temp;

	struct polyVert *v0;
	struct polyVert *v1;
	struct polyVert *tv;

	temp.n = 0;

	// Skip near and far clipping planes ?
	for (j = 0; j <= HNG64_BOTTOM; j++)
	{
		for (i = 0; i < p->n; i++)
		{
			k = (i+1) % p->n; // Index of next vertex

			v0 = &p->vert[i];
			v1 = &p->vert[k];

			tv = &temp.vert[temp.n];

			if (Inside(v0, j) && Inside(v1, j))                         // Edge is completely inside the volume...
			{
				memcpy(tv, v1, sizeof(struct polyVert));
				temp.n++;
			}

			else if (Inside(v0, j) && !Inside(v1, j))                   // Edge goes from in to out...
			{
				Intersect(v0, v1, tv, j);
				temp.n++;
			}

			else if (!Inside(v0, j) && Inside(v1, j))                   // Edge goes from out to in...
			{
				Intersect(v0, v1, tv, j);
				memcpy(&temp.vert[temp.n+1], v1, sizeof(struct polyVert));
				temp.n+=2;
			}
		}

		p->n = temp.n;

		for (i = 0; i < temp.n; i++)
		{
			memcpy(&p->vert[i], &temp.vert[i], sizeof(struct polyVert));
		}

		temp.n = 0;
	}
}



/*********************************************************************/
/**   FillSmoothTexPCHorizontalLine                                 **/
/**     Input: Color Buffer (framebuffer), depth buffer, width and  **/
/**            height of framebuffer, starting, and ending values   **/
/**            for x and y, constant y.  Fills horizontally with    **/
/**            z,r,g,b interpolation.                               **/
/**                                                                 **/
/**     Output: none                                                **/
/*********************************************************************/
inline void hng64_state::FillSmoothTexPCHorizontalLine(
											const polygonRasterOptions& prOptions,
											int x_start, int x_end, int y, float z_start, float z_delta,
											float w_start, float w_delta, float r_start, float r_delta,
											float g_start, float g_delta, float b_start, float b_delta,
											float s_start, float s_delta, float t_start, float t_delta)
{
	float*  db = &(m_depthBuffer3d[(y * m_screen->visible_area().max_x) + x_start]);
	UINT32* cb = &(m_colorBuffer3d[(y * m_screen->visible_area().max_x) + x_start]);

	UINT8 paletteEntry = 0;
	float t_coord, s_coord;
	const UINT8 *gfx = m_texturerom;   
	const UINT8 *textureOffset = &gfx[prOptions.texIndex * 1024 * 1024];

	for (; x_start <= x_end; x_start++)
	{
		if (z_start < (*db))
		{
			// MULTIPLY BACK THROUGH BY W
			t_coord = t_start / w_start;
			s_coord = s_start / w_start;

			if ((prOptions.debugColor & 0xff000000) == 0x01000000)
			{
				// UV COLOR MODE
				*cb = rgb_t(255, (UINT8)(s_coord*255.0f), (UINT8)(t_coord*255.0f), (UINT8)(0));
				*db = z_start;
			}
			else if ((prOptions.debugColor & 0xff000000) == 0x02000000)
			{
				// Lit
				*cb = rgb_t(255, (UINT8)(r_start/w_start), (UINT8)(g_start/w_start), (UINT8)(b_start/w_start));
				*db = z_start;
			}
			else if ((prOptions.debugColor & 0xff000000) == 0xff000000)
			{
				// DEBUG COLOR MODE
				*cb = prOptions.debugColor;
				*db = z_start;
			}
			else
			{
				float textureS = 0.0f;
				float textureT = 0.0f;

				// Standard & Half-Res textures
				if (prOptions.texType == 0x0)
				{
					textureS = s_coord * 1024.0f;
					textureT = t_coord * 1024.0f;
				}
				else if (prOptions.texType == 0x1)
				{
					textureS = s_coord * 512.0f;
					textureT = t_coord * 512.0f;
				}

				// stuff in mode 1 here already looks good?
				// Small-Page textures
				if (prOptions.texPageSmall == 2)
				{
					textureT = fmod(textureT, 256.0f);
					textureS = fmod(textureS, 256.0f);

					textureT += (256.0f * (prOptions.texPageHorizOffset>>1));
					textureS += (256.0f * (prOptions.texPageVertOffset>>1));
				}
				else if (prOptions.texPageSmall == 3)
				{
					textureT = fmod(textureT, 128.0f);
					textureS = fmod(textureS, 128.0f);

					textureT += (128.0f * (prOptions.texPageHorizOffset>>0));
					textureS += (128.0f * (prOptions.texPageVertOffset>>0));
				}

				paletteEntry = textureOffset[((int)textureS)*1024 + (int)textureT];

				// Naieve Alpha Implementation (?) - don't draw if you're at texture index 0...
				if (paletteEntry != 0)
				{
					// The color out of the texture
					paletteEntry %= prOptions.palPageSize;
					rgb_t color = m_palette->pen(prOptions.palOffset + paletteEntry);

					// Apply the lighting
					float rIntensity = (r_start/w_start) / 255.0f;
					float gIntensity = (g_start/w_start) / 255.0f;
					float bIntensity = (b_start/w_start) / 255.0f;
					float red   = color.r()   * rIntensity;
					float green = color.g() * gIntensity;
					float blue  = color.b()  * bIntensity;

					// Clamp and finalize
					red = color.r() + red;
					green = color.g() + green;
					blue = color.b() + blue;

					if (red >= 255) red = 255;
					if (green >= 255) green = 255;
					if (blue >= 255) blue = 255;

					color = rgb_t(255, (UINT8)red, (UINT8)green, (UINT8)blue);

					*cb = color;
					*db = z_start;
				}
			}
		}
		db++;
		cb++;
		z_start += z_delta;
		w_start += w_delta;
		r_start += r_delta;
		g_start += g_delta;
		b_start += b_delta;
		s_start += s_delta;
		t_start += t_delta;
	}
}

//----------------------------------------------------------------------------
// Given 3D triangle ABC in screen space with clipped coordinates within the following
// bounds: x in [0,W], y in [0,H], z in [0,1]. The origin for (x,y) is in the bottom
// left corner of the pixel grid. z=0 is the near plane and z=1 is the far plane,
// so lesser values are closer. The coordinates of the pixels are evenly spaced
// in x and y 1 units apart starting at the bottom-left pixel with coords
// (0.5,0.5). In other words, the pixel sample point is in the center of the
// rectangular grid cell containing the pixel sample. The framebuffer has
// dimensions width x height (WxH). The Color buffer is a 1D array (row-major
// order) with 3 unsigned chars per pixel (24-bit color). The Depth buffer is
// a 1D array (also row-major order) with a float value per pixel
// For a pixel location (x,y) we can obtain
// the Color and Depth array locations as: Color[(((int)y)*W+((int)x))*3]
// (for the red value, green is offset +1, and blue is offset +2 and
// Depth[((int)y)*W+((int)x)]. Fills the pixels contained in the triangle
// with the global current color and the properly linearly interpolated depth
// value (performs Z-buffer depth test before writing new pixel).
// Pixel samples that lie inside the triangle edges are filled with
// a bias towards the minimum values (samples that lie exactly on a triangle
// edge are filled only for minimum x values along a horizontal span and for
// minimum y values, samples lying on max values are not filled).
// Per-vertex colors are RGB floating point triplets in [0.0,255.0]. The vertices
// include their w-components for use in linearly interpolating perspectively
// correct color (RGB) and texture-coords (st) across the face of the triangle.
// A texture image of RGB floating point triplets of size TWxWH is also given.
// Texture colors are normalized RGB values in [0,1].
//   clamp and repeat wrapping modes : Wrapping={0,1}
//   nearest and bilinear filtering: Filtering={0,1}
//   replace and modulate application modes: Function={0,1}
//---------------------------------------------------------------------------
void hng64_state::RasterizeTriangle_SMOOTH_TEX_PC(
											float A[4], float B[4], float C[4],
											float Ca[3], float Cb[3], float Cc[3], // PER-VERTEX RGB COLORS
											float Ta[2], float Tb[2], float Tc[2], // PER-VERTEX (S,T) TEX-COORDS
											const polygonRasterOptions& prOptions)
{
	// Get our order of points by increasing y-coord
	float *p_min = ((A[1] <= B[1]) && (A[1] <= C[1])) ? A : ((B[1] <= A[1]) && (B[1] <= C[1])) ? B : C;
	float *p_max = ((A[1] >= B[1]) && (A[1] >= C[1])) ? A : ((B[1] >= A[1]) && (B[1] >= C[1])) ? B : C;
	float *p_mid = ((A != p_min) && (A != p_max)) ? A : ((B != p_min) && (B != p_max)) ? B : C;

	// Perspectively correct color interpolation, interpolate r/w, g/w, b/w, then divide by 1/w at each pixel (A[3] = 1/w)
	float ca[3], cb[3], cc[3];
	float ta[2], tb[2], tc[2];

	float *c_min;
	float *c_mid;
	float *c_max;

	// We must keep the tex coords straight with the point ordering
	float *t_min;
	float *t_mid;
	float *t_max;

	// Find out control points for y, this divides the triangle into upper and lower
	int   y_min;
	int   y_max;
	int   y_mid;

	// Compute the slopes of each line, and color this is used to determine the interpolation
	float x1_slope;
	float x2_slope;
	float z1_slope;
	float z2_slope;
	float w1_slope;
	float w2_slope;
	float r1_slope;
	float r2_slope;
	float g1_slope;
	float g2_slope;
	float b1_slope;
	float b2_slope;
	float s1_slope;
	float s2_slope;
	float t1_slope;
	float t2_slope;

	// Compute the t values used in the equation Ax = Ax + (Bx - Ax)*t
	// We only need one t, because it is only used to compute the start.
	// Create storage for the interpolated x and z values for both lines
	// also for the RGB interpolation
	float t;
	float x1_interp;
	float z1_interp;
	float w1_interp;
	float r1_interp;
	float g1_interp;
	float b1_interp;
	float s1_interp;
	float t1_interp;

	float x2_interp;
	float z2_interp;
	float w2_interp;
	float r2_interp;
	float g2_interp;
	float b2_interp;
	float s2_interp;
	float t2_interp;

	// Create storage for the horizontal interpolation of z and RGB color and its starting points
	// This is used to fill the triangle horizontally
	int   x_start,     x_end;
	float z_interp_x,  z_delta_x;
	float w_interp_x,  w_delta_x;
	float r_interp_x,  r_delta_x;
	float g_interp_x,  g_delta_x;
	float b_interp_x,  b_delta_x;
	float s_interp_x,  s_delta_x;
	float t_interp_x,  t_delta_x;

	ca[0] = Ca[0]; ca[1] = Ca[1]; ca[2] = Ca[2];
	cb[0] = Cb[0]; cb[1] = Cb[1]; cb[2] = Cb[2];
	cc[0] = Cc[0]; cc[1] = Cc[1]; cc[2] = Cc[2];

	// Perspectively correct tex interpolation, interpolate s/w, t/w, then divide by 1/w at each pixel (A[3] = 1/w)
	ta[0] = Ta[0]; ta[1] = Ta[1];
	tb[0] = Tb[0]; tb[1] = Tb[1];
	tc[0] = Tc[0]; tc[1] = Tc[1];

	// We must keep the colors straight with the point ordering
	c_min = (p_min == A) ? ca : (p_min == B) ? cb : cc;
	c_mid = (p_mid == A) ? ca : (p_mid == B) ? cb : cc;
	c_max = (p_max == A) ? ca : (p_max == B) ? cb : cc;

	// We must keep the tex coords straight with the point ordering
	t_min = (p_min == A) ? ta : (p_min == B) ? tb : tc;
	t_mid = (p_mid == A) ? ta : (p_mid == B) ? tb : tc;
	t_max = (p_max == A) ? ta : (p_max == B) ? tb : tc;

	// Find out control points for y, this divides the triangle into upper and lower
	y_min  = (((int)p_min[1]) + 0.5 >= p_min[1]) ? (int)p_min[1] : ((int)p_min[1]) + 1;
	y_max  = (((int)p_max[1]) + 0.5 <  p_max[1]) ? (int)p_max[1] : ((int)p_max[1]) - 1;
	y_mid  = (((int)p_mid[1]) + 0.5 >= p_mid[1]) ? (int)p_mid[1] : ((int)p_mid[1]) + 1;

	// Compute the slopes of each line, and color this is used to determine the interpolation
	x1_slope = (p_max[0] - p_min[0]) / (p_max[1] - p_min[1]);
	x2_slope = (p_mid[0] - p_min[0]) / (p_mid[1] - p_min[1]);
	z1_slope = (p_max[2] - p_min[2]) / (p_max[1] - p_min[1]);
	z2_slope = (p_mid[2] - p_min[2]) / (p_mid[1] - p_min[1]);
	w1_slope = (p_max[3] - p_min[3]) / (p_max[1] - p_min[1]);
	w2_slope = (p_mid[3] - p_min[3]) / (p_mid[1] - p_min[1]);
	r1_slope = (c_max[0] - c_min[0]) / (p_max[1] - p_min[1]);
	r2_slope = (c_mid[0] - c_min[0]) / (p_mid[1] - p_min[1]);
	g1_slope = (c_max[1] - c_min[1]) / (p_max[1] - p_min[1]);
	g2_slope = (c_mid[1] - c_min[1]) / (p_mid[1] - p_min[1]);
	b1_slope = (c_max[2] - c_min[2]) / (p_max[1] - p_min[1]);
	b2_slope = (c_mid[2] - c_min[2]) / (p_mid[1] - p_min[1]);
	s1_slope = (t_max[0] - t_min[0]) / (p_max[1] - p_min[1]);
	s2_slope = (t_mid[0] - t_min[0]) / (p_mid[1] - p_min[1]);
	t1_slope = (t_max[1] - t_min[1]) / (p_max[1] - p_min[1]);
	t2_slope = (t_mid[1] - t_min[1]) / (p_mid[1] - p_min[1]);

	// Compute the t values used in the equation Ax = Ax + (Bx - Ax)*t
	// We only need one t, because it is only used to compute the start.
	// Create storage for the interpolated x and z values for both lines
	// also for the RGB interpolation
	t = (((float)y_min) + 0.5 - p_min[1]) / (p_max[1] - p_min[1]);
	x1_interp = p_min[0] + (p_max[0] - p_min[0]) * t;
	z1_interp = p_min[2] + (p_max[2] - p_min[2]) * t;
	w1_interp = p_min[3] + (p_max[3] - p_min[3]) * t;
	r1_interp = c_min[0] + (c_max[0] - c_min[0]) * t;
	g1_interp = c_min[1] + (c_max[1] - c_min[1]) * t;
	b1_interp = c_min[2] + (c_max[2] - c_min[2]) * t;
	s1_interp = t_min[0] + (t_max[0] - t_min[0]) * t;
	t1_interp = t_min[1] + (t_max[1] - t_min[1]) * t;

	t = (((float)y_min) + 0.5 - p_min[1]) / (p_mid[1] - p_min[1]);
	x2_interp = p_min[0] + (p_mid[0] - p_min[0]) * t;
	z2_interp = p_min[2] + (p_mid[2] - p_min[2]) * t;
	w2_interp = p_min[3] + (p_mid[3] - p_min[3]) * t;
	r2_interp = c_min[0] + (c_mid[0] - c_min[0]) * t;
	g2_interp = c_min[1] + (c_mid[1] - c_min[1]) * t;
	b2_interp = c_min[2] + (c_mid[2] - c_min[2]) * t;
	s2_interp = t_min[0] + (t_mid[0] - t_min[0]) * t;
	t2_interp = t_min[1] + (t_mid[1] - t_min[1]) * t;

	// First work on the bottom half of the triangle
	// I'm using y_min as the incrementer because it saves space and we don't need it anymore
	for (; y_min < y_mid; y_min++) {
		// We always want to fill left to right, so we have 2 main cases
		// Compute the integer starting and ending points and the appropriate z by
		// interpolating.  Remember the pixels are in the middle of the grid, i.e. (0.5,0.5,0.5)
		if (x1_interp < x2_interp) {
			x_start    = ((((int)x1_interp) + 0.5) >= x1_interp) ? (int)x1_interp : ((int)x1_interp) + 1;
			x_end      = ((((int)x2_interp) + 0.5) <  x2_interp) ? (int)x2_interp : ((int)x2_interp) - 1;
			z_delta_x  = (z2_interp - z1_interp) / (x2_interp - x1_interp);
			w_delta_x  = (w2_interp - w1_interp) / (x2_interp - x1_interp);
			r_delta_x  = (r2_interp - r1_interp) / (x2_interp - x1_interp);
			g_delta_x  = (g2_interp - g1_interp) / (x2_interp - x1_interp);
			b_delta_x  = (b2_interp - b1_interp) / (x2_interp - x1_interp);
			s_delta_x  = (s2_interp - s1_interp) / (x2_interp - x1_interp);
			t_delta_x  = (t2_interp - t1_interp) / (x2_interp - x1_interp);
			t          = (x_start + 0.5 - x1_interp) / (x2_interp - x1_interp);
			z_interp_x = z1_interp + (z2_interp - z1_interp) * t;
			w_interp_x = w1_interp + (w2_interp - w1_interp) * t;
			r_interp_x = r1_interp + (r2_interp - r1_interp) * t;
			g_interp_x = g1_interp + (g2_interp - g1_interp) * t;
			b_interp_x = b1_interp + (b2_interp - b1_interp) * t;
			s_interp_x = s1_interp + (s2_interp - s1_interp) * t;
			t_interp_x = t1_interp + (t2_interp - t1_interp) * t;

		} else {
			x_start    = ((((int)x2_interp) + 0.5) >= x2_interp) ? (int)x2_interp : ((int)x2_interp) + 1;
			x_end      = ((((int)x1_interp) + 0.5) <  x1_interp) ? (int)x1_interp : ((int)x1_interp) - 1;
			z_delta_x  = (z1_interp - z2_interp) / (x1_interp - x2_interp);
			w_delta_x  = (w1_interp - w2_interp) / (x1_interp - x2_interp);
			r_delta_x  = (r1_interp - r2_interp) / (x1_interp - x2_interp);
			g_delta_x  = (g1_interp - g2_interp) / (x1_interp - x2_interp);
			b_delta_x  = (b1_interp - b2_interp) / (x1_interp - x2_interp);
			s_delta_x  = (s1_interp - s2_interp) / (x1_interp - x2_interp);
			t_delta_x  = (t1_interp - t2_interp) / (x1_interp - x2_interp);
			t          = (x_start + 0.5 - x2_interp) / (x1_interp - x2_interp);
			z_interp_x = z2_interp + (z1_interp - z2_interp) * t;
			w_interp_x = w2_interp + (w1_interp - w2_interp) * t;
			r_interp_x = r2_interp + (r1_interp - r2_interp) * t;
			g_interp_x = g2_interp + (g1_interp - g2_interp) * t;
			b_interp_x = b2_interp + (b1_interp - b2_interp) * t;
			s_interp_x = s2_interp + (s1_interp - s2_interp) * t;
			t_interp_x = t2_interp + (t1_interp - t2_interp) * t;
		}

		// Pass the horizontal line to the filler, this could be put in the routine
		// then interpolate for the next values of x and z
		FillSmoothTexPCHorizontalLine( prOptions,
			x_start, x_end, y_min, z_interp_x, z_delta_x, w_interp_x, w_delta_x,
			r_interp_x, r_delta_x, g_interp_x, g_delta_x, b_interp_x, b_delta_x,
			s_interp_x, s_delta_x, t_interp_x, t_delta_x);
		x1_interp += x1_slope;   z1_interp += z1_slope;
		x2_interp += x2_slope;   z2_interp += z2_slope;
		r1_interp += r1_slope;   r2_interp += r2_slope;
		g1_interp += g1_slope;   g2_interp += g2_slope;
		b1_interp += b1_slope;   b2_interp += b2_slope;
		w1_interp += w1_slope;   w2_interp += w2_slope;
		s1_interp += s1_slope;   s2_interp += s2_slope;
		t1_interp += t1_slope;   t2_interp += t2_slope;
	}

	// Now do the same thing for the top half of the triangle.
	// We only need to recompute the x2 line because it changes at the midpoint
	x2_slope = (p_max[0] - p_mid[0]) / (p_max[1] - p_mid[1]);
	z2_slope = (p_max[2] - p_mid[2]) / (p_max[1] - p_mid[1]);
	w2_slope = (p_max[3] - p_mid[3]) / (p_max[1] - p_mid[1]);
	r2_slope = (c_max[0] - c_mid[0]) / (p_max[1] - p_mid[1]);
	g2_slope = (c_max[1] - c_mid[1]) / (p_max[1] - p_mid[1]);
	b2_slope = (c_max[2] - c_mid[2]) / (p_max[1] - p_mid[1]);
	s2_slope = (t_max[0] - t_mid[0]) / (p_max[1] - p_mid[1]);
	t2_slope = (t_max[1] - t_mid[1]) / (p_max[1] - p_mid[1]);

	t = (((float)y_mid) + 0.5 - p_mid[1]) / (p_max[1] - p_mid[1]);
	x2_interp = p_mid[0] + (p_max[0] - p_mid[0]) * t;
	z2_interp = p_mid[2] + (p_max[2] - p_mid[2]) * t;
	w2_interp = p_mid[3] + (p_max[3] - p_mid[3]) * t;
	r2_interp = c_mid[0] + (c_max[0] - c_mid[0]) * t;
	g2_interp = c_mid[1] + (c_max[1] - c_mid[1]) * t;
	b2_interp = c_mid[2] + (c_max[2] - c_mid[2]) * t;
	s2_interp = t_mid[0] + (t_max[0] - t_mid[0]) * t;
	t2_interp = t_mid[1] + (t_max[1] - t_mid[1]) * t;

	// We've seen this loop before haven't we?
	// I'm using y_mid as the incrementer because it saves space and we don't need it anymore
	for (; y_mid <= y_max; y_mid++) {
		if (x1_interp < x2_interp) {
			x_start    = ((((int)x1_interp) + 0.5) >= x1_interp) ? (int)x1_interp : ((int)x1_interp) + 1;
			x_end      = ((((int)x2_interp) + 0.5) <  x2_interp) ? (int)x2_interp : ((int)x2_interp) - 1;
			z_delta_x  = (z2_interp - z1_interp) / (x2_interp - x1_interp);
			w_delta_x  = (w2_interp - w1_interp) / (x2_interp - x1_interp);
			r_delta_x  = (r2_interp - r1_interp) / (x2_interp - x1_interp);
			g_delta_x  = (g2_interp - g1_interp) / (x2_interp - x1_interp);
			b_delta_x  = (b2_interp - b1_interp) / (x2_interp - x1_interp);
			s_delta_x  = (s2_interp - s1_interp) / (x2_interp - x1_interp);
			t_delta_x  = (t2_interp - t1_interp) / (x2_interp - x1_interp);
			t          = (x_start + 0.5 - x1_interp) / (x2_interp - x1_interp);
			z_interp_x = z1_interp + (z2_interp - z1_interp) * t;
			w_interp_x = w1_interp + (w2_interp - w1_interp) * t;
			r_interp_x = r1_interp + (r2_interp - r1_interp) * t;
			g_interp_x = g1_interp + (g2_interp - g1_interp) * t;
			b_interp_x = b1_interp + (b2_interp - b1_interp) * t;
			s_interp_x = s1_interp + (s2_interp - s1_interp) * t;
			t_interp_x = t1_interp + (t2_interp - t1_interp) * t;

		} else {
			x_start    = ((((int)x2_interp) + 0.5) >= x2_interp) ? (int)x2_interp : ((int)x2_interp) + 1;
			x_end      = ((((int)x1_interp) + 0.5) <  x1_interp) ? (int)x1_interp : ((int)x1_interp) - 1;
			z_delta_x  = (z1_interp - z2_interp) / (x1_interp - x2_interp);
			w_delta_x  = (w1_interp - w2_interp) / (x1_interp - x2_interp);
			r_delta_x  = (r1_interp - r2_interp) / (x1_interp - x2_interp);
			g_delta_x  = (g1_interp - g2_interp) / (x1_interp - x2_interp);
			b_delta_x  = (b1_interp - b2_interp) / (x1_interp - x2_interp);
			s_delta_x  = (s1_interp - s2_interp) / (x1_interp - x2_interp);
			t_delta_x  = (t1_interp - t2_interp) / (x1_interp - x2_interp);
			t          = (x_start + 0.5 - x2_interp) / (x1_interp - x2_interp);
			z_interp_x = z2_interp + (z1_interp - z2_interp) * t;
			w_interp_x = w2_interp + (w1_interp - w2_interp) * t;
			r_interp_x = r2_interp + (r1_interp - r2_interp) * t;
			g_interp_x = g2_interp + (g1_interp - g2_interp) * t;
			b_interp_x = b2_interp + (b1_interp - b2_interp) * t;
			s_interp_x = s2_interp + (s1_interp - s2_interp) * t;
			t_interp_x = t2_interp + (t1_interp - t2_interp) * t;
		}

		// Pass the horizontal line to the filler, this could be put in the routine
		// then interpolate for the next values of x and z
		FillSmoothTexPCHorizontalLine( prOptions,
			x_start, x_end, y_mid, z_interp_x, z_delta_x, w_interp_x, w_delta_x,
			r_interp_x, r_delta_x, g_interp_x, g_delta_x, b_interp_x, b_delta_x,
			s_interp_x, s_delta_x, t_interp_x, t_delta_x);
		x1_interp += x1_slope;   z1_interp += z1_slope;
		x2_interp += x2_slope;   z2_interp += z2_slope;
		r1_interp += r1_slope;   r2_interp += r2_slope;
		g1_interp += g1_slope;   g2_interp += g2_slope;
		b1_interp += b1_slope;   b2_interp += b2_slope;
		w1_interp += w1_slope;   w2_interp += w2_slope;
		s1_interp += s1_slope;   s2_interp += s2_slope;
		t1_interp += t1_slope;   t2_interp += t2_slope;
	}
}

void hng64_state::drawShaded( struct polygon *p)
{
	// The perspective-correct texture divide...
	// !!! There is a very good chance the HNG64 hardware does not do perspective-correct texture-mapping !!!
	int j;
	for (j = 0; j < p->n; j++)
	{
		p->vert[j].clipCoords[3] = 1.0f / p->vert[j].clipCoords[3];
		p->vert[j].light[0]      = p->vert[j].light[0]     * p->vert[j].clipCoords[3];
		p->vert[j].light[1]      = p->vert[j].light[1]     * p->vert[j].clipCoords[3];
		p->vert[j].light[2]      = p->vert[j].light[2]     * p->vert[j].clipCoords[3];
		p->vert[j].texCoords[0]  = p->vert[j].texCoords[0] * p->vert[j].clipCoords[3];
		p->vert[j].texCoords[1]  = p->vert[j].texCoords[1] * p->vert[j].clipCoords[3];
	}

	// Set up the struct that will pass the polygon's options around.
	polygonRasterOptions prOptions;
	prOptions.texType = p->texType;
	prOptions.texIndex = p->texIndex;
	prOptions.palOffset = p->palOffset;
	prOptions.palPageSize = p->palPageSize;
	prOptions.debugColor = p->debugColor;
	prOptions.texPageSmall = p->texPageSmall;
	prOptions.texPageHorizOffset = p->texPageHorizOffset;
	prOptions.texPageVertOffset = p->texPageVertOffset;

	for (j = 1; j < p->n-1; j++)
	{
		RasterizeTriangle_SMOOTH_TEX_PC(
										p->vert[0].clipCoords, p->vert[j].clipCoords, p->vert[j+1].clipCoords,
										p->vert[0].light,      p->vert[j].light,      p->vert[j+1].light,
										p->vert[0].texCoords,  p->vert[j].texCoords,  p->vert[j+1].texCoords,
										prOptions);
	}
}

