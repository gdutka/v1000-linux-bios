/** @file
  Chipset Setup Configuration Data

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifdef _IMPORT_CHIPSET_COMMON_SETUP_

//==========================================================================================================
//==========================================================================================================
//----------------------------------------------------------------------------------------------------------
// The following area is used by Chipset team to modify.
// The total size of variables in this part(Chipset_Start ~ Chipset_End) are fixed(300bytes). That means if
// you need to add or remove variables, please modify the ChipsetRSV buffer size as well.
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// Please setup up the Chipset dependent variables here.
//----------------------------------------------------------------------------------------------------------
//Chipset_Start
//Offset(230),
  UINT8         ChipsetRSV1;                           //Offset 230
  UINT8         ChipsetRSV2;                           //Offset 231
  UINT8         WireLessSwitch;                        //Offset 232
  UINT8         ChipsetRSV4;                           //Offset 233
  UINT8         ChipsetRSV5;                           //Offset 234
  UINT8         PowerNowControl;                       //Offset 235
  UINT8         MpSupport;                             //Offset 236
  UINT8         StibpStatus;                           //Offset 237
  UINT8         EnableSVM;                             //Offset 238
  UINT8         SVMLock;                               //Offset 239
  UINT8         ChipsetRSV11;                          //Offset 240
  UINT8         ChipsetRSV12;                          //Offset 241
  UINT8         ChipsetRSV13;                          //Offset 242
  UINT8         CpuPstateSetting;                      //Offset 243
  UINT8         ChipsetRSV15;                          //Offset 244
  UINT8         ChipsetRSV16;                          //Offset 245
  UINT8         OnboardLanSwitch;                      //Offset 246
  UINT8         ChipsetRSV18;                          //Offset 247
  UINT8         ChipsetRSV19;                          //Offset 248
  UINT8         ChipsetRSV20;                          //Offset 249
  UINT8         AHCIMODE;                              //Offset 250
  UINT8         ChipsetRSV22;                          //Offset 251
  UINT8         XgpSupport;                            //Offset 252
  UINT8         cmosBoardinfo;                         //Offset 253
  UINT8         ThermalFanControl;                     //Offset 254
  UINT8         amdLightSensor;                        //Offset 255
  UINT8         EnableTPM;                             //Offset 256
  UINT8         EnableHDMIAudio;                       //Offset 257
  UINT8         SpiOrLpcTPM;                           //Offset 258
  UINT8         amdLassoSupport;                       //Offset 259
  UINT8         amdCpuHysteresisInfo;                  //Offset 260
  UINT8         amdCpuAC0;                             //Offset 261
  UINT8         amdCpuAC1;                             //Offset 262
  UINT8         ErasefTPMNV;                           //Offset 263
  UINT8         ChipsetRSV35;                          //Offset 264
  UINT8         ChipsetRSV36;                          //Offset 265
  UINT8         amdCpuAL0;                             //Offset 266
  UINT8         amdCpuAL1;                             //Offset 267
  UINT8         ChipsetRSV39;                          //Offset 268
  UINT8         ChipsetRSV40;                          //Offset 269
  UINT8         ChipsetRSV41;                          //Offset 270
  UINT8         ChipsetRSV42;                          //Offset 271
  UINT8         ChipsetRSV43;                          //Offset 272
  UINT8         ChipsetRSV44;                          //Offset 273
  UINT8         ChipsetRSV45;                          //Offset 274
  UINT8         ChipsetRSV46;                          //Offset 275
  UINT8         ChipsetRSV47;                          //Offset 276
  UINT8         ChipsetRSV48;                          //Offset 277
  UINT8         amdLcdPanelType;                       //Offset 278
  UINT8         ChipsetRSV50;                          //Offset 279
  UINT8         ChipsetRSV51;                          //Offset 280
  UINT8         amdExternalUSBController;              //Offset 281
  UINT8         amdFusionUtilityControl;               //Offset 282
  UINT8         ChipsetRSV54;                          //Offset 283
  UINT8         ChipsetRSV55;                          //Offset 284
  UINT8         ReclaimLongrunTest;                    //Offset 285
  UINT8         ChipsetRSV57;                          //Offset 286
  UINT8         ChipsetRSV58;                          //Offset 287
  UINT8         AmdKernelDebugSupport;                 //Offset 288
  UINT8         ChipsetRSV60;                          //Offset 289
  UINT8         ChipsetRSV61;                          //Offset 290
  UINT8         ChipsetRSV62;                          //Offset 291
  UINT8         ChipsetRSV63;                          //Offset 292
  UINT8         ChipsetRSV64;                          //Offset 293
  UINT8         ChipsetRSV65;                          //Offset 294
  UINT8         ChipsetRSV66;                          //Offset 295
  UINT8         ChipsetRSV67;                          //Offset 296
  UINT8         amdThermalSensorSelect;                //Offset 297
  UINT8         ChipsetRSV69;                          //Offset 298
  UINT8         ChipsetRSV70;                          //Offset 299
  UINT8         ChipsetRSV71;                          //Offset 300
  UINT8         ChipsetRSV72;                          //Offset 301
  UINT8         ChipsetRSV73;                          //Offset 302
  UINT8         ChipsetRSV74;                          //Offset 303
  UINT8         ChipsetRSV75;                          //Offset 304
  UINT8         ChipsetRSV76;                          //Offset 305
  UINT8         ChipsetRSV77;                          //Offset 306
  UINT8         ChipsetRSV78;                          //Offset 307
  UINT8         ChipsetRSV79;                          //Offset 308
  UINT8         ChipsetRSV80;                          //Offset 309
  UINT8         ChipsetRSV81;                          //Offset 310
  UINT8         ChipsetRSV82;                          //Offset 311
  UINT8         ChipsetRSV83;                          //Offset 312
  UINT8         amdOddZeroPowerControl;                //Offset 313
  UINT8         OscSwitch;                             //Offset 314
  UINT8         ChipsetRSV86;                          //Offset 315
  UINT8         UsbPort0;                              //Offset 316
  UINT8         UsbPort1;                              //Offset 317
  UINT8         UsbPort2;                              //Offset 318
  UINT8         UsbPort3;                              //Offset 319
  UINT8         UsbPort4;                              //Offset 320
  UINT8         UsbPort5;                              //Offset 321
  UINT8         Ehci0;                                 //Offset 322
  UINT8         Ehci1;                                 //Offset 323
  UINT8         Ehci2;                                 //Offset 324
  UINT8         Sata;                                  //Offset 325
  UINT8         ChipsetRSV97;                          //Offset 326
  UINT8         ForceRaid;                             //Offset 327
  UINT8         SataPort0Power;                        //Offset 328
  UINT8         SataPort1Power;                        //Offset 329
  UINT8         ChipsetRSV101;                         //Offset 330
  UINT8         ChipsetRSV102;                         //Offset 331
  UINT8         ChipsetRSV103;                         //Offset 332
  UINT8         SMMCoreLock;                           //Offset 333
  UINT8         ChipsetRSV105;                         //Offset 334
  UINT8         APUGpp0SpeedMode;                      //Offset 335
  UINT8         APUGpp0LinkAspm;                       //Offset 336
  UINT8         APUGpp1SpeedMode;                      //Offset 337
  UINT8         APUGpp1LinkAspm;                       //Offset 338
  UINT8         APUGpp2SpeedMode;                      //Offset 339
  UINT8         APUGpp2LinkAspm;                       //Offset 340
  UINT8         APUGpp3SpeedMode;                      //Offset 341
  UINT8         APUGpp3LinkAspm;                       //Offset 342
  UINT8         ChipsetRSV114;                         //Offset 343
  UINT8         ChipsetRSV115;                         //Offset 344
  UINT8         ChipsetRSV116;                         //Offset 345
  UINT8         GnbPciePsppPolicy;                     //Offset 346
  UINT8         APUGpp0;                               //Offset 347
  UINT8         APUGpp0StartLaneID;                    //Offset 348
  UINT8         APUGpp0EndLaneID;                      //Offset 349
  UINT8         APUGpp0Hotplug;                        //Offset 350
  UINT8         APUGpp1;                               //Offset 351
  UINT8         APUGpp1StartLaneID;                    //Offset 352
  UINT8         APUGpp1EndLaneID;                      //Offset 353
  UINT8         APUGpp1Hotplug;                        //Offset 354
  UINT8         APUGpp2;                               //Offset 355
  UINT8         APUGpp2StartLaneID;                    //Offset 356
  UINT8         APUGpp2EndLaneID;                      //Offset 357
  UINT8         APUGpp2Hotplug;                        //Offset 358
  UINT8         APUGpp3;                               //Offset 359
  UINT8         APUGpp3StartLaneID;                    //Offset 360
  UINT8         APUGpp3EndLaneID;                      //Offset 361
  UINT8         APUGpp3Hotplug;                        //Offset 362
  UINT8         APUGpp4;                               //Offset 363
  UINT8         APUGpp4StartLaneID;                    //Offset 364
  UINT8         APUGpp4EndLaneID;                      //Offset 365
  UINT8         APUGpp4Hotplug;                        //Offset 366
  UINT8         APUGpp5;                               //Offset 367
  UINT8         APUGpp6;                               //Offset 368
  UINT8         KbcSupport;                            //Offset 369
  UINT8         ChipsetRSV141;                         //Offset 370
  UINT8         ChipsetRSV142;                         //Offset 371
  UINT8         ChipsetRSV143;                         //Offset 372
  UINT8         ChipsetRSV144;                         //Offset 373
  UINT8         ChipsetRSV145;                         //Offset 374
  UINT8         ChipsetRSV146;                         //Offset 375
  UINT8         ChipsetRSV147;                         //Offset 376
  UINT8         ChipsetRSV148;                         //Offset 377
  UINT8         S5LongRunTest;                         //Offset 378
  UINT8         FastRecoverySwitch;                    //Offset 379
  UINT8         Gpp0Mux;                               //Offset 380
  UINT8         PeiGop;                                //Offset 381
  UINT8         BrightnessSwitch;                      //Offset 382
  UINT8         ClkReq;                                //Offset 383
  UINT8         IrActivityLedEn;                       //Offset 384
  UINT8         ChipsetRSV156;                         //Offset 385
  UINT8         ChipsetRSV157;                         //Offset 386
  UINT8         amdSystemBootWithPS0;                  //Offset 387
  UINT8         TravisLCtrl;                           //Offset 388
  UINT8         AdaptiveS4En;                          //Offset 389
  UINT8         BrightnessControlMethod;               //Offset 390
  UINT8         UnusedGppClkOff;                       //Offset 391
  UINT8         SpreadSpectrumSwitch;                  //Offset 392
  UINT8         WlanWwan;                              //Offset 393
  UINT8         ChipsetRSV165;                         //Offset 394
  UINT8         BlueToothEn;                           //Offset 395
  UINT8         GpsSensor;                             //Offset 396
  UINT8         NfcSensor;                             //Offset 397
  UINT8         UnusedClkOff;                          //Offset 398
  UINT8         SdPowerControl;                        //Offset 399
  UINT8         DtSlotPowerControl;                    //Offset 340
  UINT8         WwanPowerEn;                           //Offset 341
  UINT8         ChipsetRSV173;                         //Offset 402
  UINT8         LanPowerControl;                       //Offset 403
  UINT8         EvalAutoDetection;                     //Offset 404
  UINT8         amdAcDcSwitch;                         //Offset 405
  UINT8         amdAcTimer;                            //Offset 406
  UINT8         amdDcTimer;                            //Offset 407
  UINT8         DtHotPlugEnable;                       //Offset 408
  UINT8         M2WLan;                                //Offset 409
  UINT8         amdClearMCAErrorsAtWarmReset;          //Offset 410
  UINT8         AmdPspSwitch;                          //Offset 411
  UINT8         MITTWITTControl;                       //Offset 412
  UINT8         ChipsetRSV184;                         //Offset 413
  UINT8         ChipsetRSV185;                         //Offset 414
  UINT8         ChipsetRSV186;                         //Offset 415
  UINT8         ChipsetRSV187;                         //Offset 416
  UINT8         ChipsetRSV188;                         //Offset 417
  UINT8         ChipsetRSV189;                         //Offset 418
  UINT8         ChipsetRSV190;                         //Offset 419
  UINT8         ChipsetRSV191;                         //Offset 420
  UINT8         ChipsetRSV192;                         //Offset 421
  UINT8         ChipsetRSV193;                         //Offset 422
  UINT8         ChipsetRSV194;                         //Offset 423
  UINT8         ChipsetRSV195;                         //Offset 424
  UINT8         DashSupport;                           //Offset 425
  UINT8         ChipsetRSV[49];                        //Offset 426 - 474
  UINT8         ThirdPartyRSV1;                        //Offset 475
  UINT8         ThirdPartyRSV2;                        //Offset 476
  UINT8         ThirdPartyRSV3;                        //Offset 477
  UINT8         ThirdPartyRSV4;                        //Offset 478
  UINT8         ThirdPartyRSV5;                        //Offset 479
  UINT8         ThirdPartyRSV6;                        //Offset 480
  UINT8         ThirdPartyRSV7;                        //Offset 481
  UINT8         ThirdPartyRSV8;                        //Offset 482
  UINT8         ThirdPartyRSV9;                        //Offset 483
  UINT8         ThirdPartyRSV10;                       //Offset 484
  UINT8         ThirdPartyRSV11;                       //Offset 485
  UINT8         ThirdPartyRSV12;                       //Offset 486
  UINT8         ThirdPartyRSV13;                       //Offset 487
  UINT8         ThirdPartyRSV14;                       //Offset 488
  UINT8         ThirdPartyRSV15;                       //Offset 489
  UINT8         ThirdPartyRSV16;                       //Offset 490
  UINT8         ThirdPartyRSV17;                       //Offset 491
  UINT8         ThirdPartyRSV18;                       //Offset 492
  UINT8         ThirdPartyRSV19;                       //Offset 493
  UINT8         ThirdPartyRSV20;                       //Offset 494
  UINT8         ThirdPartyRSV21;                       //Offset 495
  UINT8         ThirdPartyRSV22;                       //Offset 496
  UINT8         ThirdPartyRSV23;                       //Offset 497
  UINT8         ThirdPartyRSV24;                       //Offset 498
  UINT8         ThirdPartyRSV25;                       //Offset 499
  UINT8         ThirdPartyRSV26;                       //Offset 500
  UINT8         ThirdPartyRSV27;                       //Offset 501
  UINT8         ThirdPartyRSV28;                       //Offset 502
  UINT8         ThirdPartyRSV29;                       //Offset 503
  UINT8         ThirdPartyRSV30;                       //Offset 504
  UINT8         ThirdPartyRSV31;                       //Offset 505
  UINT8         ThirdPartyRSV32;                       //Offset 506
  UINT8         ThirdPartyRSV33;                       //Offset 507
  UINT8         ThirdPartyRSV34;                       //Offset 508
  UINT8         SetupVariableInvalid;                  //Offset 509
//>>EnableAbove4GBMmio**
  UINT8         EnableAbove4GBMmio;                    //Offset 510
  UINT8         ThirdPartyRSV[19];                     //Offset 511 - 528
//<<EnableAbove4GBMmio**
//Chipset_End
//----------------------------------------------------------------------------------------------------------
// End of area for Chipset team use.
//----------------------------------------------------------------------------------------------------------

#endif