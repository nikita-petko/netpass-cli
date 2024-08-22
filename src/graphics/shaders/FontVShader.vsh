//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//

// Input registers map
#define aPosition       v0
#define aColor          v1
#define aTexCoord       v2

// Output registers map
#define vPosition       o0
#define vQuaternion     o1
#define vView           o2
#define vTexCoord       o3

// Constant registers map
#define MATRIX_Proj         c0
#define MATRIX_Proj0        c0
#define MATRIX_Proj1        c1
#define MATRIX_Proj2        c2
#define MATRIX_Proj3        c3

#define MATRIX_ModelView    c4
#define MATRIX_ModelView0   c4
#define MATRIX_ModelView00  c4
#define MATRIX_ModelView01  c5
#define MATRIX_ModelView02  c6
#define MATRIX_ModelView03  c7

// Temporary registers map
#define TEMP0               r0
#define TEMP1               r1
#define TEMP2               r2
#define TEMP3               r3
#define TEMP4               r4

#define TEMP_TEX0           r10
#define TEMP_TEX1           r11
#define TEMP_TEX2           r12
#define TEMP_TEX3           r13
#define TEMP_TANG           r13
#define TEMP_NORM           r14
#define TEMP_VIEW           r15
#define TEMP_POS            r12

// Constant registers
#define CONSTANT            c93
def     CONSTANT[0], 0.0, 1.0, 2.0, 3.0
def     CONSTANT[1], 0.125, 0.00390625, 0.5, 0.25
#define CONST_0                 CONSTANT[0].x
#define CONST_1                 CONSTANT[0].y
#define CONST_2                 CONSTANT[0].z
#define CONST_3                 CONSTANT[0].w
#define CONST_HALF              CONSTANT[1].z
#define CONST_QUARTER           CONSTANT[1].w
#define CONST_1_0               CONSTANT[0].yx
#define CONST_1__4              CONSTANT[1].w
#define CONST_1__8              CONSTANT[1].x
#define CONST_1__256            CONSTANT[1].y

#pragma bind_symbol(aPosition.xyzw,v0,v0)
#pragma bind_symbol(aColor.xyzw,v1,v1)
#pragma bind_symbol(aTexCoord.xyz,v2,v2)

#pragma output_map (position, o0)
#pragma output_map (color, o1)
#pragma output_map (texture0, o2.xyz)

#pragma bind_symbol(uProjection,c0,c3)
#pragma bind_symbol(uModelView,c4,c7)

l_position:
    m4x4    TEMP_VIEW,          aPosition,          MATRIX_ModelView
    mov     TEMP_POS,           MATRIX_Proj2
    add     TEMP_POS,           TEMP_POS,           MATRIX_Proj3
    mul     TEMP_POS,           TEMP_POS,           -CONST_HALF
    dp4     vPosition.x,        TEMP_VIEW,          MATRIX_Proj0
    dp4     vPosition.y,        TEMP_VIEW,          MATRIX_Proj1
    dp4     vPosition.z,        TEMP_VIEW,          TEMP_POS
    dp4     vPosition.w,        TEMP_VIEW,          MATRIX_Proj3
    ret
    nop

main:
    call    l_position
    mov     o1, aColor
    mov     o2, aTexCoord.xyxy
    end
endmain:

