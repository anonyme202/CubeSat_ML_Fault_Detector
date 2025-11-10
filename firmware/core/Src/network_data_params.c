/**
  ******************************************************************************
  * @file    network_data_params.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2025-11-10T22:59:55+0100
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "network_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_network_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_network_weights_array_u64[163] = {
  0x3efbab66be5aae72U, 0x3e58d6533ed38a14U, 0x3e8b43223e8cb8efU, 0xbf6717fcbe87beceU,
  0xbeda395fbeafc283U, 0xbf2a50c53e1d75d6U, 0x3f3335e8be43191dU, 0xbf607381bf386cd2U,
  0x3e8890a7bf02ea62U, 0x3e3527363ede4e3aU, 0xbf64ec5a3df8da95U, 0x3f8063fdbd7d390cU,
  0x3df26acfbe981e4cU, 0x3eb23cedbdda9e72U, 0x3d50dcad3e95c0c1U, 0xbf5615573f700ea5U,
  0x3d22b8b73db6f359U, 0x3eca64fdbe32609cU, 0x3f485c0dbd73890bU, 0xbd9073c6bf46588bU,
  0xbdd05064be73e7a2U, 0x3da28011be78ef41U, 0x3f1746fabf14e296U, 0xbf4c4fffbef9757cU,
  0x3ea2cb6d3c497883U, 0x3cf631d2bed0c12fU, 0xbec5dd6fbf19574cU, 0x3eff3b45beba6c22U,
  0x3da7c2b23e3afff0U, 0xbe6a2352be87b524U, 0xbeea022b3d92964cU, 0x3dbf3f6abe404649U,
  0xbe01be133eb2f418U, 0xbf10e678bcc57865U, 0x3e9242923ec4f166U, 0x3e62906dbf3be115U,
  0x3e743575beb971cdU, 0xbd9da3c13d41a687U, 0xbeb358c3bf62f31fU, 0x3f0ad8ffbe995491U,
  0xbd259d00bf06ad3bU, 0xbe13e9c63f1ba764U, 0x3cbd0f1cbe13a38aU, 0xbe974d2b3eb8698eU,
  0xbb7d6052bd889c4dU, 0x3e72d10ebef303a7U, 0xbe7f1c493f77feafU, 0xbec32a1cbebbaaeaU,
  0xbe5f694b3ec0c272U, 0xbdda02673d56a08dU, 0x3ea72364bf372d8bU, 0xbf15925f3f42312fU,
  0xbdbf99823ea3435fU, 0x3f046194bd8719c9U, 0x3eeb51683d84e404U, 0xbef301aabf467e50U,
  0xbe0290b93eb83922U, 0xbf04a7473db7f71dU, 0x3f30c9bd3f30923bU, 0xbeb2dc9bbe9cb462U,
  0x3ed983dfbeb45a31U, 0x3ddefde83efdb6b6U, 0x3e8f8a103b73ad13U, 0x3f26b6fabe993d14U,
  0x3ef2eced3ee441b1U, 0x3f08809ebe223305U, 0x3ef209df3ed4ddc0U, 0x3bfb1df13e822f86U,
  0x3e5e80733ecef479U, 0x3ef21ae4be497addU, 0x3ec621f03eb7ff4dU, 0xbe9fad133ede6ef5U,
  0x3e68cb5e3d55bbe1U, 0xbe7627973f82b24fU, 0x3e19dd253e0cf6f2U, 0x3f3022c23f65db9dU,
  0x3f34ed02be425dc9U, 0x3e865df33e500d3dU, 0xbd8280ef3ec40dcdU, 0x3c8f4597bf14fd60U,
  0x3f2502ec3ebe612dU, 0x3d8372d4bdd72d32U, 0x3f6cb7c73e932743U, 0xbe8c3c4f3d7d5e35U,
  0x3ca24cf03f1006b2U, 0x3eebe0d5bf0ce95dU, 0x3f11028b3e4a7eb3U, 0xbf2d57683f28497cU,
  0xbe9fe426be81cc95U, 0x3f531708be9b8b83U, 0x3e5a4d2bbf04fb4cU, 0xbe9ba1df3f0ed103U,
  0xbbf64de53ddfe542U, 0x3db9cb033eb739efU, 0xbf02e1d13f5863cfU, 0xbeac21d0bd8ee166U,
  0x3e93dd463f603c2dU, 0x3e9dfd88be0323e4U, 0x3e4638023ef59620U, 0x3f23aae53e10b695U,
  0xbdcc6c973f3ac9bdU, 0x3f23681abba1c6c2U, 0x3f38f304bf56d04dU, 0x3f25c0613f3b1910U,
  0x3e997e983ef0f715U, 0x3f792f9b3e3e3d5eU, 0xbe0ad7ba3e6409a3U, 0xbe95e494bf3386c3U,
  0xbd01c4a7bd6db511U, 0x3f386dd13f1b4a43U, 0xbe90120b3e362a18U, 0x3ef3c6a43f697269U,
  0xbe0a11863f033628U, 0xbc20c7083e5d7f03U, 0xbd8f2e6ebe941238U, 0x3e03029ebf1d64e5U,
  0x3e19bcb0bdf0f0f4U, 0x3de5ea123f12772aU, 0xbbcedeb0bde4889eU, 0x3ebc0d7abde45ea3U,
  0xbe049b2abe6b9f44U, 0x3f4b6301be88d582U, 0xbeb1e6e7bf028645U, 0xbe98c8d63d5540f0U,
  0x3f26dc00bf02c009U, 0xbc453516be6a2169U, 0x3de9b4713f90b5dfU, 0xbd1b1ab43d88a430U,
  0x3eb8b1b83f0ada98U, 0x3e53fe79bf29953bU, 0xbe0e2fa43f18aa55U, 0x3e993829bed64c94U,
  0x3ada90cb3e4c2694U, 0x3f28e958beb2ded6U, 0x3f1767b3bd496232U, 0xbee198523e81e661U,
  0x3e643f80bdc21718U, 0x3e75319fbbf8e790U, 0xbe0d1db83ee0e186U, 0x3e91868a3d4fa0c2U,
  0x3f84be49bf1b6827U, 0x3f0f9a78bf4dd611U, 0xbf043473bcf61ebbU, 0x3f8dddbdbf4af8bdU,
  0x3eacff253e8cc3dcU, 0xbea2ee5d3e7fa472U, 0x3e460458bf6afa9fU, 0xbe9091a43eb7187cU,
  0xbf5641f83dec9a3fU, 0x3edaf0e4bed71765U, 0x3f472e873e073e2aU, 0xbf63850cbf02cb93U,
  0xbf78f32d3f4b28e9U, 0x3d2cb2b4bf84711eU, 0x3d0a9a3fbf5a3789U, 0x3d87d93cbdf3c780U,
  0xbe2bdcf3bf8d760bU, 0xbf15e9ee3f1366d5U, 0xbe803d543e9004b7U, 0xbd6c19c03f56d933U,
  0xbe79b2293e2a7085U, 0x3d4a130cbe631615U, 0xbddb548cU,
};


ai_handle g_network_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_network_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

