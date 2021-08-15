/*
===========================================================================
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2015, OpenJK contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

// Filename:-	mdx_format.h
//
// DO NOT UPDATE THIS FILE IN ANY WAY WHATSOEVER WITHOUT TELLING ME (-Ste),
//	BECAUSE THE MASTER COPY IS IN A DIFFERENT SOURCESAFE DATABASE AND WILL
//	JUST GET PASTED OVER THIS ONE WHENEVER I CHANGE IT.
//
//
//
// MDX file format (typically uses file extension GLX for mesh, and GLA for anim/skeleton file)
//
// Notes:
//
//	- All offset fields are relative to the address of the structure they occur in
//	- So far, the only external symbol needed is MAX_QPATH, plus the typedefs for vec3_t, vec2_t etc

#ifndef MDX_FORMAT_H
#define MDX_FORMAT_H


#define MDXM_IDENT			(('M'<<24)+('G'<<16)+('L'<<8)+'2')
#define MDXA_IDENT			(('A'<<24)+('G'<<16)+('L'<<8)+'2')

#define MAX_TAGNAME_BYTES	32	// matches MDR, can be changed if nec.

//
// normal version numbers...
//
#define MDXM_VERSION 6
#define MDXA_VERSION 6

// (Note that since there is now a "<modelname>_info.txt" file written out by carcass any changes made in here that
//		introduce new data should also be reflected in the info-output)

// 32 bit-flags for ghoul2 bone properties...  (all undefined fields will be blank)
//
#define G2BONEFLAG_ALWAYSXFORM	0x00000001

// same thing but for surfaces...  (Carcass will only generate 1st 2 flags, others are ingame
//
#define G2SURFACEFLAG_ISBOLT		0x00000001
#define G2SURFACEFLAG_OFF			0x00000002	// saves strcmp()ing for "_off" in surface names
#define G2SURFACEFLAG_SPARE0		0x00000004	// future-expansion fields, saves invalidating models if we add more
#define G2SURFACEFLAG_SPARE1		0x00000008	//
#define G2SURFACEFLAG_SPARE2		0x00000010	//
#define G2SURFACEFLAG_SPARE3		0x00000020	//
#define G2SURFACEFLAG_SPARE4		0x00000040	//
#define G2SURFACEFLAG_SPARE5		0x00000080	//
//
#define G2SURFACEFLAG_NODESCENDANTS 0x00000100	// ingame-stuff, never generated by Carcass....
#define G2SURFACEFLAG_GENERATED		0x00000200	//



// triangle side-ordering stuff for tags...
//
#define iG2_TRISIDE_MIDDLE				1
#define iG2_TRISIDE_LONGEST				0
#define iG2_TRISIDE_SHORTEST			2

#define fG2_BONEWEIGHT_RECIPROCAL_MULT	((float)(1.0f/1023.0f))
#define iG2_BITS_PER_BONEREF			5
#define iMAX_G2_BONEREFS_PER_SURFACE	(1<<iG2_BITS_PER_BONEREF)	// (32)
#define iMAX_G2_BONEWEIGHTS_PER_VERT	4	// can't just be blindly increased, affects cache size etc

#define iG2_BONEWEIGHT_TOPBITS_SHIFT	((iG2_BITS_PER_BONEREF * iMAX_G2_BONEWEIGHTS_PER_VERT) - 8)	// 8 bits because of 8 in the BoneWeight[] array entry
#define iG2_BONEWEIGHT_TOPBITS_AND		0x300	// 2 bits, giving 10 total, or 10 bits, for 1023/1024 above


#define sDEFAULT_GLA_NAME "*default"	// used when making special simple ghoul2 models, usually from MD3 files


////////////////////////////////////
//
// these structs are defined here purely because of structure dependancy order...
//
/*
#ifdef __cplusplus
struct mdxmWeight_t
#else
typedef struct
#endif
{
	int			boneIndex;		// these are indexes into the surface boneReferences, not the global bone index
	float		boneWeight;		// not the global per-frame bone list

	// I'm defining this '<' operator so this struct can be used with an STL <set>...
	// (note that I've defined it using '>' internally, so it sorts with higher weights being "less", for distance weight-culling
	//
	#ifdef __cplusplus
	bool operator < (const mdxmWeight_t& _X) const {return (boneWeight>_X.boneWeight);}
	#endif
}
#ifndef __cplusplus
mdxmWeight_t
#endif
;
*/
/*
#ifdef __cplusplus
struct mdxaCompBone_t
#else
typedef struct
#endif
{
	unsigned char Comp[24];		// MC_COMP_BYTES is in MatComp.h, but don't want to couple

	// I'm defining this '<' operator so this struct can be used as an STL <map> key...
	//
	#ifdef __cplusplus
	bool operator < (const mdxaCompBone_t& _X) const {return (memcmp(Comp,_X.Comp,sizeof(Comp))<0);}
	#endif
}
#ifndef __cplusplus
mdxaCompBone_t
#endif
;
*/
#ifdef __cplusplus
struct mdxaCompQuatBone_t
#else
typedef struct
#endif
{
	unsigned char Comp[14];

	// I'm defining this '<' operator so this struct can be used as an STL <map> key...
	//
	#ifdef __cplusplus
	bool operator < (const mdxaCompQuatBone_t& X) const {return (memcmp(Comp,X.Comp,sizeof(Comp))<0);}
	#endif
}
#ifndef __cplusplus
mdxaCompQuatBone_t
#endif
;


#ifndef MDXABONEDEF
typedef struct {
	float matrix[3][4];


#ifdef __cplusplus
	void sg_export(
		ojk::SavedGameHelper& saved_game) const
	{
		saved_game.write<float>(matrix);
	}

	void sg_import(
		ojk::SavedGameHelper& saved_game, int32_t version)
	{
		saved_game.read<float>(matrix);
	}
#endif // __cplusplus
} mdxaBone_t;
#endif

////////////////////////////////////






// mdxHeader_t  - this contains the header for the file, with sanity checking and version checking, plus number of lod's to be expected
//
typedef struct {
	//
	// ( first 3 fields are same format as MD3/MDR so we can apply easy model-format-type checks )
	//
	int			ident;				// "IDP3" = MD3, "RDM5" = MDR, "2LGM"(GL2 Mesh) = MDX   (cruddy char order I know, but I'm following what was there in other versions)
	int			version;			// 1,2,3 etc as per format revision
	char		name[MAX_QPATH];	// model name (eg "models/players/marine.glm")	// note: extension supplied
	char		animName[MAX_QPATH];// name of animation file this mesh requires	// note: extension missing
	int			animIndex;			// filled in by game (carcass defaults it to 0)

	int			numBones;			// (for ingame version-checks only, ensure we don't ref more bones than skel file has)

	int			numLODs;
	int			ofsLODs;

	int			numSurfaces;		// now that surfaces are drawn hierarchically, we have same # per LOD
	int			ofsSurfHierarchy;

	int			ofsEnd;				// EOF, which of course gives overall file size
} mdxmHeader_t;


// for each surface (doesn't actually need a struct for this, just makes source clearer)
// {
		typedef struct
		{
			int offsets[1];		// variable sized (mdxmHeader_t->numSurfaces), each offset points to a mdxmSurfHierarchy_t below
		} mdxmHierarchyOffsets_t;
// }

// for each surface...	(mdxmHeader_t->numSurfaces)
// {
		// mdxmSurfHierarchy_t - contains hierarchical info for surfaces...

		typedef struct {
			char		name[MAX_QPATH];
			unsigned int flags;
			char		shader[MAX_QPATH];
			int			shaderIndex;		// for in-game use (carcass defaults to 0)
			int			parentIndex;		// this points to the index in the file of the parent surface. -1 if null/root
			int			numChildren;		// number of surfaces which are children of this one
			int			childIndexes[1];	// [mdxmSurfHierarch_t->numChildren] (variable sized)
		} mdxmSurfHierarchy_t;	// struct size = (int)( &((mdxmSurfHierarch_t *)0)->childIndexes[ mdxmSurfHierarch_t->numChildren ] );
// }


// for each LOD...  (mdxmHeader_t->numLODs)
// {
		// mdxLOD_t - this contains the header for this LOD. Contains num of surfaces, offset to surfaces and offset to next LOD. Surfaces are shader sorted, so each surface = 1 shader

		typedef struct {
			// (used to contain numSurface/ofsSurfaces fields, but these are same per LOD level now)
			//
			int			ofsEnd;				// offset to next LOD
		} mdxmLOD_t;


		typedef struct {	// added in GLM version 3 for ingame use at Jake's request
			int offsets[1];		// variable sized (mdxmHeader_t->numSurfaces), each offset points to surfaces below
		} mdxmLODSurfOffset_t;


		// for each surface... (mdxmHeader_t->numSurfaces)
		// {
				// mdxSurface_t - reuse of header format containing surface name, number of bones, offset to poly data and number of polys, offset to vertex information, and number of verts. NOTE offsets are relative to this header.

				typedef struct {
					int			ident;				// this one field at least should be kept, since the game-engine may switch-case (but currently=0 in carcass)

					int			thisSurfaceIndex;	// 0...mdxmHeader_t->numSurfaces-1 (because of how ingame renderer works)

					int			ofsHeader;			// this will be a negative number, pointing back to main header

					int			numVerts;
					int			ofsVerts;

					int			numTriangles;
					int			ofsTriangles;

					// Bone references are a set of ints representing all the bones
					// present in any vertex weights for this surface.  This is
					// needed because a model may have surfaces that need to be
					// drawn at different sort times, and we don't want to have
					// to re-interpolate all the bones for each surface.
					//
					int			numBoneReferences;
					int			ofsBoneReferences;

					int			ofsEnd;				// next surface follows

				} mdxmSurface_t;


				// for each triangle...	(mdxmSurface_t->numTriangles)
				// {
						// mdxTriangle_t - contains indexes into verts. One struct entry per poly.

						typedef struct {
							int			indexes[3];
						} mdxmTriangle_t;
				// }


				// for each vert... (mdxmSurface_t->numVerts)
				// {
						// mdxVertex_t - this is an array with number of verts from the surface definition as its bounds. It contains normal info, texture coors and number of weightings for this bone
						// (this is now kept at 32 bytes for cache-aligning)
						typedef struct {
							vec3_t			normal;
							vec3_t			vertCoords;

							// packed int...
							unsigned int	uiNmWeightsAndBoneIndexes;	// 32 bits.  format:
																		// 31 & 30:  0..3 (= 1..4) weight count
																		// 29 & 28 (spare)
																		//	2 bit pairs at 20,22,24,26 are 2-bit overflows from 4 BonWeights below (20=[0], 22=[1]) etc)
																		//  5-bits each (iG2_BITS_PER_BONEREF) for boneweights
							// effectively a packed int, each bone weight converted from 0..1 float to 0..255 int...
							//  promote each entry to float and multiply by fG2_BONEWEIGHT_RECIPROCAL_MULT to convert.
							byte			BoneWeightings[iMAX_G2_BONEWEIGHTS_PER_VERT];	// 4

						} mdxmVertex_t;

				// } vert

#ifdef __cplusplus

// these are convenience functions that I can invoked in code. Do NOT change them (because this is a shared file),
//	but if you want to copy the logic out and use your own versions then fine...
//
static inline int G2_GetVertWeights( const mdxmVertex_t *pVert )
{
	int iNumWeights = (pVert->uiNmWeightsAndBoneIndexes >> 30)+1;	// 1..4 count

	return iNumWeights;
}

static inline int G2_GetVertBoneIndex( const mdxmVertex_t *pVert, const int iWeightNum)
{
	int iBoneIndex = (pVert->uiNmWeightsAndBoneIndexes>>(iG2_BITS_PER_BONEREF*iWeightNum))&((1<<iG2_BITS_PER_BONEREF)-1);

	return iBoneIndex;
}

static inline float G2_GetVertBoneWeight( const mdxmVertex_t *pVert, const int iWeightNum, float &fTotalWeight, int iNumWeights )
{
	float fBoneWeight;

	if (iWeightNum == iNumWeights-1)
	{
		fBoneWeight = 1.0f-fTotalWeight;
	}
	else
	{
		int iTemp = pVert->BoneWeightings[iWeightNum];
			iTemp|= (pVert->uiNmWeightsAndBoneIndexes >> (iG2_BONEWEIGHT_TOPBITS_SHIFT+(iWeightNum*2)) ) & iG2_BONEWEIGHT_TOPBITS_AND;

		fBoneWeight = fG2_BONEWEIGHT_RECIPROCAL_MULT * iTemp;
		fTotalWeight += fBoneWeight;
	}

	return fBoneWeight;
}
#endif
				// for each vert... (mdxmSurface_t->numVerts)  (seperated from mdxmVertex_t struct for cache reasons)
				// {
						// mdxVertex_t - this is an array with number of verts from the surface definition as its bounds. It contains normal info, texture coors and number of weightings for this bone

						typedef struct {
							vec2_t			texCoords;
						} mdxmVertexTexCoord_t;

				// } vert


		// } surface
// } LOD



//----------------------------------------------------------------------------
// seperate file here for animation data...
//


// mdxaHeader_t  - this contains the header for the file, with sanity checking and version checking, plus number of lod's to be expected
//
typedef struct {
	//
	// ( first 3 fields are same format as MD3/MDR so we can apply easy model-format-type checks )
	//
	int			ident;				// 	"IDP3" = MD3, "RDM5" = MDR, "2LGA"(GL2 Anim) = MDXA
	int			version;			// 1,2,3 etc as per format revision
	//
	char		name[MAX_QPATH];	// GLA name (eg "skeletons/marine")	// note: extension missing
	float		fScale;				// will be zero if build before this field was defined, else scale it was built with

	// frames and bones are shared by all levels of detail
	//
	int			numFrames;
	int			ofsFrames;			// points at mdxaFrame_t array
	int			numBones;			// (no offset to these since they're inside the frames array)
	int			ofsCompBonePool;	// offset to global compressed-bone pool that all frames use
	int			ofsSkel;			// offset to mdxaSkel_t info

	int			ofsEnd;				// EOF, which of course gives overall file size

} mdxaHeader_t;


// for each bone... (doesn't actually need a struct for this, just makes source clearer)
// {
		typedef struct
		{
			int offsets[1];		// variable sized (mdxaHeader_t->numBones), each offset points to an mdxaSkel_t below
		} mdxaSkelOffsets_t;
// }



// for each bone...	 (mdxaHeader_t->numBones)
// {
		// mdxaSkel_t - contains hierarchical info only...

		typedef struct {
			char		name[MAX_QPATH];	// name of bone
			unsigned int flags;
			int			parent;				// index of bone that is parent to this one, -1 = NULL/root
			mdxaBone_t	BasePoseMat;		// base pose
			mdxaBone_t	BasePoseMatInv;		// inverse, to save run-time calc
			int			numChildren;		// number of children bones
			int			children[1];		// [mdxaSkel_t->numChildren] (variable sized)
		} mdxaSkel_t;	// struct size = (int)( &((mdxaSkel_t *)0)->children[ mdxaSkel_t->numChildren ] );
// }


	// (offset @ mdxaHeader_t->ofsFrames)
	//
	//  array of 3 byte indices here (hey, 25% saving over 4-byte really adds up)...
	//
	//
	// access as follows to get the index for a given <iFrameNum, iBoneNum>
	//
	// (iFrameNum * mdxaHeader_t->numBones * 3) + (iBoneNum * 3)
	//
	// Then convert the three byte int at that location.
	// This struct is used for easy searches.
	typedef struct
	{
		byte    iIndex[3];
	} mdxaIndex_t;
	//
	// (note that there's then an alignement-pad here to get the next struct back onto 32-bit alignement)
	//
	// this index then points into the following...


// Compressed-bone pool that all frames use  (mdxaHeader_t->ofsCompBonePool)  (defined at end because size unknown until end)
// for each bone in pool (unknown number, no actual total stored at the moment)...
// {
		// mdxaCompBone_t  (defined at file top because of struct dependancy)
// }

//---------------------------------------------------------------------------


#endif	// #ifndef MDX_FORMAT_H

//////////////////////// eof ///////////////////////



