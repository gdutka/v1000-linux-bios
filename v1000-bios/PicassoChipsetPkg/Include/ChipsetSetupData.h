/** @file
  Chipset Setup Configuration Data

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifdef _IMPORT_CHIPSET_SETUP_

//==========================================================================================================
//==========================================================================================================
//----------------------------------------------------------------------------------------------------------
// The following area is used by Chipset team to modify.
// The total size of variables in this part(Chipset_Start ~ Chipset_End) are fixed(230bytes). That means if
// you need to add or remove variables, please modify the ChipsetRSV buffer size as well.
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// Please setup up the Chipset dependent variables here.
//----------------------------------------------------------------------------------------------------------
// Chipset_Start
// Offset(230),
  UINT8         ChipsetRSV1;           //Offset 230
  UINT8         ChipsetRSV2;           //Offset 231
  UINT8         WireLessSwitch;        //Offset 232
  UINT8         ChipsetRSV4;           //Offset 233
  UINT8         ChipsetRSV5;           //Offset 234
  UINT8         ChipsetRSV6;           //Offset 235
  UINT8         ChipsetRSV7;           //Offset 236
  UINT8         ChipsetRSV8;           //Offset 237
  UINT8         ChipsetRSV9;           //Offset 238
  UINT8         ChipsetRSV10;          //Offset 239
  UINT8         ChipsetRSV11;          //Offset 240
  UINT8         ChipsetRSV12;          //Offset 241
  UINT8         ChipsetRSV13;          //Offset 242
  UINT8         ChipsetRSV14;          //Offset 243
  UINT8         ChipsetRSV15;          //Offset 244
  UINT8         ChipsetRSV16;          //Offset 245
  UINT8         ChipsetRSV17;          //Offset 246
  UINT8         ChipsetRSV18;          //Offset 247
  UINT8         ChipsetRSV19;          //Offset 248
  UINT8         ChipsetRSV20;          //Offset 249
  UINT8         AHCIMODE;              //Offset 250
  UINT8         ChipsetRSV22;          //Offset 251
  UINT8         ChipsetRSV23;          //Offset 252
  UINT8         ChipsetRSV24;          //Offset 253
  UINT8         ThermalFanControl;     //Offset 254
  UINT8         amdLightSensor;        //Offset 255
  UINT8         EnableTPM;             //Offset 256
  UINT8         EnableHDMIAudio;       //Offset 257
  UINT8         ChipsetRSV29;          //Offset 258
  UINT8         ChipsetRSV30;          //Offset 259
  UINT8         amdCpuHysteresisInfo;  //Offset 260
  UINT8         amdCpuAC0;             //Offset 261
  UINT8         amdCpuAC1;             //Offset 262
  UINT8         ChipsetRSV34;          //Offset 263
  UINT8         ChipsetRSV35;          //Offset 264
  UINT8         ChipsetRSV36;          //Offset 265
  UINT8         amdCpuAL0;             //Offset 266
  UINT8         amdCpuAL1;             //Offset 267
  UINT8         ChipsetRSV39;          //Offset 268
  UINT8         ChipsetRSV40;          //Offset 269
  UINT8         ChipsetRSV41;          //Offset 270
  UINT8         ChipsetRSV42;          //Offset 271
  UINT8         ChipsetRSV43;          //Offset 272
  UINT8         ChipsetRSV44;          //Offset 273
  UINT8         ChipsetRSV45;          //Offset 274
  UINT8         ChipsetRSV46;          //Offset 275
  UINT8         ChipsetRSV47;          //Offset 276
  UINT8         ChipsetRSV48;          //Offset 277
  UINT8         amdLcdPanelType;       //Offset 278
  UINT8         ChipsetRSV50;          //Offset 279
  UINT8         ChipsetRSV51;          //Offset 280
  UINT8         amdExternalUSBController; //Offset 281
  UINT8         ChipsetRSV53;          //Offset 282
  UINT8         ChipsetRSV54;          //Offset 283
  UINT8         ChipsetRSV55;          //Offset 284
  UINT8         ChipsetRSV56;          //Offset 285
  UINT8         ChipsetRSV57;          //Offset 286
  UINT8         ChipsetRSV58;          //Offset 287
  UINT8         ChipsetRSV59;          //Offset 288
  UINT8         ChipsetRSV60;          //Offset 289
  UINT8         ChipsetRSV61;          //Offset 290
  UINT8         ChipsetRSV62;          //Offset 291
  UINT8         ChipsetRSV63;          //Offset 292
  UINT8         ChipsetRSV64;          //Offset 293
  UINT8         ChipsetRSV65;          //Offset 294
  UINT8         ChipsetRSV66;          //Offset 295
  UINT8         ChipsetRSV67;          //Offset 296
  UINT8         amdThermalSensorSelect;//Offset 297
  UINT8         ChipsetRSV69;          //Offset 298
  UINT8         ChipsetRSV70;          //Offset 299
  UINT8         ChipsetRSV71;          //Offset 300
  UINT8         ChipsetRSV72;          //Offset 301
  UINT8         ChipsetRSV73;          //Offset 302
  UINT8         ChipsetRSV74;          //Offset 303
  UINT8         ChipsetRSV75;          //Offset 304
  UINT8         ChipsetRSV76;          //Offset 305
  UINT8         ChipsetRSV77;          //Offset 306
  UINT8         ChipsetRSV78;          //Offset 307
  UINT8         ChipsetRSV79;          //Offset 308
  UINT8         ChipsetRSV80;          //Offset 309
  UINT8         ChipsetRSV81;          //Offset 310
  UINT8         ChipsetRSV82;          //Offset 311
  UINT8         ChipsetRSV83;          //Offset 312
  UINT8         ChipsetRSV84;          //Offset 313
  UINT8         OscSwitch;             //Offset 314
  UINT8         ChipsetRSV86;          //Offset 315
  UINT8         ChipsetRSV87;          //Offset 316
  UINT8         ChipsetRSV88;          //Offset 317
  UINT8         Ohci0;                 //Offset 318
  UINT8         Ohci1;                 //Offset 319
  UINT8         Ohci2;                 //Offset 320
  UINT8         Ohci3;                 //Offset 321
  UINT8         Ehci0;                 //Offset 322
  UINT8         Ehci1;                 //Offset 323
  UINT8         Ehci2;                 //Offset 324
  UINT8         ChipsetRSV96;          //Offset 325
  UINT8         Azalia;                //Offset 326
  UINT8         ChipsetRSV98;          //Offset 327
  UINT8         ChipsetRSV99;          //Offset 328
  UINT8         ChipsetRSV100;         //Offset 329
  UINT8         ChipsetRSV101;         //Offset 330
  UINT8         ChipsetRSV102;         //Offset 331
  UINT8         ChipsetRSV103;         //Offset 332
  UINT8         ChipsetRSV104;         //Offset 333
  UINT8         ChipsetRSV105;         //Offset 334
  UINT8         APUGpp0SpeedMode;      //Offset 335
  UINT8         APUGpp0LinkAspm;       //Offset 336
  UINT8         APUGpp1SpeedMode;      //Offset 337
  UINT8         APUGpp1LinkAspm;       //Offset 338
  UINT8         APUGpp2SpeedMode;      //Offset 339
  UINT8         APUGpp2LinkAspm;       //Offset 340
  UINT8         APUGpp3SpeedMode;      //Offset 341
  UINT8         APUGpp3LinkAspm;       //Offset 342
  UINT8         ChipsetRSV114;         //Offset 343
  UINT8         ChipsetRSV115;         //Offset 344
  UINT8         ChipsetRSV116;         //Offset 345
  UINT8         GnbPciePsppPolicy;     //Offset 346
  UINT8         APUGpp0;               //Offset 347
  UINT8         APUGpp0StartLaneID;    //Offset 348
  UINT8         APUGpp0EndLaneID;      //Offset 349
  UINT8         APUGpp0Hotplug;        //Offset 350
  UINT8         APUGpp1;               //Offset 351
  UINT8         APUGpp1StartLaneID;    //Offset 352
  UINT8         APUGpp1EndLaneID;      //Offset 353
  UINT8         APUGpp1Hotplug;        //Offset 354
  UINT8         APUGpp2;               //Offset 355
  UINT8         APUGpp2StartLaneID;    //Offset 356
  UINT8         APUGpp2EndLaneID;      //Offset 357
  UINT8         APUGpp2Hotplug;        //Offset 358
  UINT8         APUGpp3;               //Offset 359
  UINT8         APUGpp3StartLaneID;    //Offset 360
  UINT8         APUGpp3EndLaneID;      //Offset 361
  UINT8         APUGpp3Hotplug;        //Offset 362
  UINT8         APUGpp4;               //Offset 363
  UINT8         APUGpp4StartLaneID;    //Offset 364
  UINT8         APUGpp4EndLaneID;      //Offset 365
  UINT8         APUGpp4Hotplug;        //Offset 366
  UINT8         ChipsetRSV138;         //Offset 367
  UINT8         ChipsetRSV139;         //Offset 368
  UINT8         ChipsetRSV140;         //Offset 369
  UINT8         ChipsetRSV141;         //Offset 370
  UINT8         ChipsetRSV142;         //Offset 371
  UINT8         ChipsetRSV143;         //Offset 372
  UINT8         ChipsetRSV144;         //Offset 373
  UINT8         ChipsetRSV145;         //Offset 374
  UINT8         ChipsetRSV146;         //Offset 375
  UINT8         ChipsetRSV147;         //Offset 376
  UINT8         ChipsetRSV148;         //Offset 377
  UINT8         ChipsetRSV149;         //Offset 378
  UINT8         ChipsetRSV150;         //Offset 379
  UINT8         Gpp0Mux;               //Offset 380
  UINT8         ChipsetRSV152;         //Offset 381
  UINT8         ChipsetRSV153;         //Offset 382
  UINT8         ClkReq;                //Offset 383
  UINT8         ChipsetRSV155;         //Offset 384
  UINT8         ChipsetRSV156;         //Offset 385
  UINT8         ChipsetRSV157;         //Offset 386
  UINT8         ChipsetRSV158;         //Offset 387
  UINT8         ChipsetRSV159;         //Offset 388
  UINT8         ChipsetRSV160;         //Offset 389
  UINT8         ChipsetRSV161;         //Offset 390
  UINT8         ChipsetRSV162;         //Offset 391
  UINT8         ChipsetRSV163;         //Offset 392
  UINT8         ChipsetRSV164;         //Offset 393
  UINT8         ChipsetRSV165;         //Offset 394
  UINT8         ChipsetRSV166;         //Offset 395
  UINT8         ChipsetRSV167;         //Offset 396
  UINT8         ChipsetRSV168;         //Offset 397
  UINT8         ChipsetRSV169;         //Offset 398
  UINT8         ChipsetRSV170;         //Offset 399
  UINT8         ChipsetRSV171;         //Offset 400
  UINT8         ChipsetRSV172;         //Offset 401
  UINT8         ChipsetRSV173;         //Offset 402
  UINT8         ChipsetRSV174;         //Offset 403
  UINT8         ChipsetRSV175;         //Offset 404
  UINT8         ChipsetRSV176;         //Offset 405
  UINT8         ChipsetRSV177;         //Offset 406
  UINT8         ChipsetRSV178;         //Offset 407
  UINT8         ChipsetRSV179;         //Offset 408
  UINT8         ChipsetRSV180;         //Offset 409
  UINT8         ChipsetRSV181;         //Offset 410
  UINT8         ChipsetRSV182;         //Offset 411
  UINT8         ChipsetRSV183;         //Offset 412
  UINT8         ChipsetRSV184;         //Offset 413
  UINT8         ChipsetRSV185;         //Offset 414
  UINT8         ChipsetRSV186;         //Offset 415
  UINT8         ChipsetRSV187;         //Offset 416
  UINT8         ChipsetRSV188;         //Offset 417
  UINT8         ChipsetRSV189;         //Offset 418
  UINT8         ChipsetRSV190;         //Offset 419
  UINT8         ChipsetRSV191;         //Offset 420
  UINT8         ChipsetRSV192;         //Offset 421
  UINT8         ChipsetRSV193;         //Offset 422
  UINT8         ChipsetRSV194;         //Offset 423
//   UINT8         ChipsetRSV195;         //Offset 424
//   UINT8         ChipsetRSV196;         //Offset 425
//   UINT8         ChipsetRSV197;         //Offset 426
//   UINT8         ChipsetRSV198;         //Offset 427
//   UINT8         ChipsetRSV199;         //Offset 428
//   UINT8         ChipsetRSV200;         //Offset 429
//   UINT8         ChipsetRSV201;         //Offset 430
//   UINT8         ChipsetRSV202;         //Offset 431
//   UINT8         ChipsetRSV203;         //Offset 432
//   UINT8         ChipsetRSV204;         //Offset 433
//   UINT8         ChipsetRSV205;         //Offset 434
//   UINT8         ChipsetRSV206;         //Offset 435
//   UINT8         ChipsetRSV207;         //Offset 436
//   UINT8         ChipsetRSV208;         //Offset 437
//   UINT8         ChipsetRSV209;         //Offset 438
//   UINT8         ChipsetRSV210;         //Offset 439
//   UINT8         ChipsetRSV211;         //Offset 440
//   UINT8         ChipsetRSV212;         //Offset 441
//   UINT8         ChipsetRSV213;         //Offset 442
//   UINT8         ChipsetRSV214;         //Offset 443
//   UINT8         ChipsetRSV215;         //Offset 444
//   UINT8         ChipsetRSV216;         //Offset 445
//   UINT8         ChipsetRSV217;         //Offset 446
//   UINT8         ChipsetRSV218;         //Offset 447
//   UINT8         ChipsetRSV219;         //Offset 448
//   UINT8         ChipsetRSV220;         //Offset 449
//   UINT8         ChipsetRSV221;         //Offset 450
//   UINT8         ChipsetRSV222;         //Offset 451
//   UINT8         ChipsetRSV223;         //Offset 452
//   UINT8         ChipsetRSV224;         //Offset 453
//   UINT8         ChipsetRSV225;         //Offset 454
//   UINT8         ChipsetRSV226;         //Offset 455
//   UINT8         ChipsetRSV227;         //Offset 456
//   UINT8         ChipsetRSV228;         //Offset 457
//   UINT8         ChipsetRSV229;         //Offset 458
//   UINT8         ChipsetRSV230;         //Offset 459
// Chipset_End
//----------------------------------------------------------------------------------------------------------
// End of area for Chipset team use. 
//----------------------------------------------------------------------------------------------------------
//==========================================================================================================
//----------------------------------------------------------------------------------------------------------
// Start area for Third Party. The following area is used by Third Party to modify. 
// The total size of variable in this part are fixed (220 bytes). That means if you need to add or remove  
// variables, please modify the ThirdPartyRSV buffer size as well.
//----------------------------------------------------------------------------------------------------------  
//Offset(460);
// AMD IDS Start
  UINT8    ThirdPartyRSV1;              //Offset 460
  UINT8    ThirdPartyRSV2;              //Offset 461
  UINT8    ThirdPartyRSV3;              //Offset 462
  UINT8    ThirdPartyRSV4;              //Offset 463
  UINT8    ThirdPartyRSV5;              //Offset 464
  UINT8    ThirdPartyRSV6;              //Offset 465
  UINT8    ThirdPartyRSV7;              //Offset 466
  UINT8    ThirdPartyRSV8;              //Offset 467
  UINT8    ThirdPartyRSV9;              //Offset 468
  UINT8    ThirdPartyRSV10;             //Offset 469
  UINT8    ThirdPartyRSV11;             //Offset 470
  UINT8    ThirdPartyRSV12;             //Offset 471
  UINT8    ThirdPartyRSV13;             //Offset 472
  UINT8    ThirdPartyRSV14;             //Offset 473
  UINT8    ThirdPartyRSV15;             //Offset 474
  UINT8    ThirdPartyRSV16;             //Offset 475
  UINT8    ThirdPartyRSV17;             //Offset 476
  UINT8    ThirdPartyRSV18;             //Offset 477
  UINT8    ThirdPartyRSV19;             //Offset 478
  UINT8    ThirdPartyRSV20;             //Offset 479
  UINT8    ThirdPartyRSV21;             //Offset 480
  UINT8    ThirdPartyRSV22;             //Offset 481
  UINT8    ThirdPartyRSV23;             //Offset 482
  UINT8    ThirdPartyRSV24;             //Offset 483
  UINT8    ThirdPartyRSV25;             //Offset 484
  UINT8    ThirdPartyRSV26;             //Offset 485
  UINT8    ThirdPartyRSV27;             //Offset 486
  UINT8    ThirdPartyRSV28;             //Offset 487
  UINT8    ThirdPartyRSV29;             //Offset 488
  UINT8    ThirdPartyRSV30;             //Offset 489
  UINT8    ThirdPartyRSV31;             //Offset 490
  UINT8    ThirdPartyRSV32;             //Offset 491
  UINT8    ThirdPartyRSV33;             //Offset 492
  UINT8    ThirdPartyRSV34;             //Offset 493
  UINT8    ThirdPartyRSV35;             //Offset 494
  UINT8    ThirdPartyRSV36;             //Offset 495
  UINT8    ThirdPartyRSV37;             //Offset 496
  UINT8    ThirdPartyRSV38;             //Offset 497
  UINT8    ThirdPartyRSV39;             //Offset 498
//   UINT8    ThirdPartyRSV40;             //Offset 499
//   UINT8    ThirdPartyRSV41;             //Offset 500
//   UINT8    ThirdPartyRSV42;             //Offset 501
//   UINT8    ThirdPartyRSV43;             //Offset 502
//   UINT8    ThirdPartyRSV44;             //Offset 503
//   UINT8    ThirdPartyRSV45;             //Offset 504
//   UINT8    ThirdPartyRSV46;             //Offset 505
//   UINT8    ThirdPartyRSV47;             //Offset 506
//   UINT8    ThirdPartyRSV48;             //Offset 507
//   UINT8    ThirdPartyRSV49;             //Offset 508
//   UINT8    ThirdPartyRSV50;             //Offset 509
//   UINT8    ThirdPartyRSV51;             //Offset 510
//   UINT8    ThirdPartyRSV52;             //Offset 511
//   UINT8    ThirdPartyRSV53;             //Offset 512
//   UINT8    ThirdPartyRSV54;             //Offset 513
//   UINT8    ThirdPartyRSV55;             //Offset 514
//   UINT8    ThirdPartyRSV56;             //Offset 515
//   UINT8    ThirdPartyRSV57;             //Offset 516
//   UINT8    ThirdPartyRSV58;             //Offset 517
//   UINT8    ThirdPartyRSV59;             //Offset 518
//   UINT8    ThirdPartyRSV60;             //Offset 519
//   UINT8    ThirdPartyRSV61;             //Offset 520
//   UINT8    ThirdPartyRSV62;             //Offset 521
//   UINT8    ThirdPartyRSV63;             //Offset 522
//   UINT8    ThirdPartyRSV64;             //Offset 523
//   UINT8    ThirdPartyRSV65;             //Offset 524
//   UINT8    ThirdPartyRSV66;             //Offset 525
//   UINT8    ThirdPartyRSV67;             //Offset 526
//   UINT8    ThirdPartyRSV68;             //Offset 527
//   UINT8    ThirdPartyRSV69;             //Offset 528
//   UINT8    ThirdPartyRSV70;             //Offset 529
//   UINT8    ThirdPartyRSV71;             //Offset 530
//   UINT8    ThirdPartyRSV72;             //Offset 531
//   UINT8    ThirdPartyRSV73;             //Offset 532
//   UINT8    ThirdPartyRSV74;             //Offset 533
//   UINT8    ThirdPartyRSV75;             //Offset 534
//   UINT8    ThirdPartyRSV76;             //Offset 535
//   UINT8    ThirdPartyRSV77;             //Offset 536
//   UINT8    ThirdPartyRSV78;             //Offset 537
//   UINT8    ThirdPartyRSV79;             //Offset 538
//   UINT8    ThirdPartyRSV80;             //Offset 539
//   UINT8    ThirdPartyRSV81;             //Offset 540
//   UINT8    ThirdPartyRSV82;             //Offset 541
//   UINT8    ThirdPartyRSV83;             //Offset 542
//   UINT8    ThirdPartyRSV84;             //Offset 543
//   UINT8    ThirdPartyRSV85;             //Offset 544
//   UINT8    ThirdPartyRSV86;             //Offset 545
//   UINT8    ThirdPartyRSV87;             //Offset 546
//   UINT8    ThirdPartyRSV88;             //Offset 547
//   UINT8    ThirdPartyRSV89;             //Offset 548
//   UINT8    ThirdPartyRSV90;             //Offset 549
//   UINT8    ThirdPartyRSV91;             //Offset 550
//   UINT8    ThirdPartyRSV92;             //Offset 551
//   UINT8    ThirdPartyRSV93;             //Offset 552
//   UINT8    ThirdPartyRSV94;             //Offset 553
//   UINT8    ThirdPartyRSV95;             //Offset 554
//   UINT8    ThirdPartyRSV96;             //Offset 555
//   UINT8    ThirdPartyRSV97;             //Offset 556
//   UINT8    ThirdPartyRSV98;             //Offset 557
//   UINT8    ThirdPartyRSV99;             //Offset 558
//   UINT8    ThirdPartyRSV100;            //Offset 559
//   UINT8    ThirdPartyRSV101;            //Offset 560
//   UINT8    ThirdPartyRSV102;            //Offset 561
//   UINT8    ThirdPartyRSV103;            //Offset 562
//   UINT8    ThirdPartyRSV104;            //Offset 563
//   UINT8    ThirdPartyRSV105;            //Offset 564
//   UINT8    ThirdPartyRSV106;            //Offset 565
//   UINT8    ThirdPartyRSV107;            //Offset 566
//   UINT8    ThirdPartyRSV108;            //Offset 567
//   UINT8    ThirdPartyRSV109;            //Offset 568
//   UINT8    ThirdPartyRSV110;            //Offset 569
//   UINT8    ThirdPartyRSV111;            //Offset 570
//   UINT8    ThirdPartyRSV112;            //Offset 571
//   UINT8    ThirdPartyRSV113;            //Offset 572
//   UINT8    ThirdPartyRSV114;            //Offset 573
//   UINT8    ThirdPartyRSV115;            //Offset 574
//   UINT8    ThirdPartyRSV116;            //Offset 575
//   UINT8    ThirdPartyRSV117;            //Offset 576
//   UINT8    ThirdPartyRSV118;            //Offset 577
//   UINT8    ThirdPartyRSV119;            //Offset 578
//   UINT8    ThirdPartyRSV120;            //Offset 579
//   UINT8    ThirdPartyRSV121;            //Offset 580
//   UINT8    ThirdPartyRSV122;            //Offset 581
//   UINT8    ThirdPartyRSV123;            //Offset 582
//   UINT8    ThirdPartyRSV124;            //Offset 583
//   UINT8    ThirdPartyRSV125;            //Offset 584
//   UINT8    ThirdPartyRSV126;            //Offset 585
//   UINT8    ThirdPartyRSV127;            //Offset 586
//   UINT8    ThirdPartyRSV128;            //Offset 587
//   UINT8    ThirdPartyRSV129;            //Offset 588
//   UINT8    ThirdPartyRSV130;            //Offset 589
//   UINT8    ThirdPartyRSV131;            //Offset 590
//   UINT8    ThirdPartyRSV132;            //Offset 591
//   UINT8    ThirdPartyRSV133;            //Offset 592
//   UINT8    ThirdPartyRSV134;            //Offset 593
//   UINT8    ThirdPartyRSV135;            //Offset 594
//   UINT8    ThirdPartyRSV136;            //Offset 595
//   UINT8    ThirdPartyRSV137;            //Offset 596
//   UINT8    ThirdPartyRSV138;            //Offset 597
//   UINT8    ThirdPartyRSV139;            //Offset 598
//   UINT8    ThirdPartyRSV140;            //Offset 599
//   UINT8    ThirdPartyRSV141;            //Offset 600
//   UINT8    ThirdPartyRSV142;            //Offset 601
//   UINT8    ThirdPartyRSV143;            //Offset 602
//   UINT8    ThirdPartyRSV144;            //Offset 603
//   UINT8    ThirdPartyRSV145;            //Offset 604
//   UINT8    ThirdPartyRSV146;            //Offset 605
//   UINT8    ThirdPartyRSV147;            //Offset 606
//   UINT8    ThirdPartyRSV148;            //Offset 607
//   UINT8    ThirdPartyRSV149;            //Offset 608
//   UINT8    ThirdPartyRSV150;            //Offset 609
//   UINT8    ThirdPartyRSV151;            //Offset 610
//   UINT8    ThirdPartyRSV152;            //Offset 611
//   UINT8    ThirdPartyRSV153;            //Offset 612
//   UINT8    ThirdPartyRSV154;            //Offset 613
//   UINT8    ThirdPartyRSV155;            //Offset 614
//   UINT8    ThirdPartyRSV156;            //Offset 615
//   UINT8    ThirdPartyRSV157;            //Offset 616
//   UINT8    ThirdPartyRSV158;            //Offset 617
//   UINT8    ThirdPartyRSV159;            //Offset 618
//   UINT8    ThirdPartyRSV160;            //Offset 619
//   UINT8    ThirdPartyRSV161;            //Offset 620
//   UINT8    ThirdPartyRSV162;            //Offset 621
//   UINT8    ThirdPartyRSV163;            //Offset 622
//   UINT8    ThirdPartyRSV164;            //Offset 623
//   UINT8    ThirdPartyRSV165;            //Offset 624
//   UINT8    ThirdPartyRSV166;            //Offset 625
//   UINT8    ThirdPartyRSV167;            //Offset 626
//   UINT8    ThirdPartyRSV168;            //Offset 627
//   UINT8    ThirdPartyRSV169;            //Offset 628
//   UINT8    ThirdPartyRSV170;            //Offset 629
//   UINT8    ThirdPartyRSV171;            //Offset 630
//   UINT8    ThirdPartyRSV172;            //Offset 631
//   UINT8    ThirdPartyRSV173;            //Offset 632
//   UINT8    ThirdPartyRSV174;            //Offset 633
//   UINT8    ThirdPartyRSV175;            //Offset 634
//   UINT8    ThirdPartyRSV176;            //Offset 635
//   UINT8    ThirdPartyRSV177;            //Offset 636
//   UINT8    ThirdPartyRSV178;            //Offset 637
//   UINT8    ThirdPartyRSV179;            //Offset 638
//   UINT8    ThirdPartyRSV180;            //Offset 639
//   UINT8    ThirdPartyRSV181;            //Offset 640
//   UINT8    ThirdPartyRSV182;            //Offset 641
//   UINT8    ThirdPartyRSV183;            //Offset 642
//   UINT8    ThirdPartyRSV184;            //Offset 643
//   UINT8    ThirdPartyRSV185;            //Offset 644
//   UINT8    ThirdPartyRSV186;            //Offset 645
//   UINT8    ThirdPartyRSV187;            //Offset 646
//   UINT8    ThirdPartyRSV188;            //Offset 647
//   UINT8    ThirdPartyRSV189;            //Offset 648
//   UINT8    ThirdPartyRSV190;            //Offset 649
//   UINT8    ThirdPartyRSV191;            //Offset 650
//   UINT8    ThirdPartyRSV192;            //Offset 651
//   UINT8    ThirdPartyRSV193;            //Offset 652
//   UINT8    ThirdPartyRSV194;            //Offset 653
//   UINT8    ThirdPartyRSV195;            //Offset 654
//   UINT8    ThirdPartyRSV196;            //Offset 655
//   UINT8    ThirdPartyRSV197;            //Offset 656
//   UINT8    ThirdPartyRSV198;            //Offset 657
//   UINT8    ThirdPartyRSV199;            //Offset 658
//   UINT8    ThirdPartyRSV200;            //Offset 659
//   UINT8    ThirdPartyRSV201;            //Offset 660
//   UINT8    ThirdPartyRSV202;            //Offset 661
//   UINT8    ThirdPartyRSV203;            //Offset 662
//   UINT8    ThirdPartyRSV204;            //Offset 663
//   UINT8    ThirdPartyRSV205;            //Offset 664
//   UINT8    ThirdPartyRSV206;            //Offset 665
//   UINT8    ThirdPartyRSV207;            //Offset 666
//   UINT8    ThirdPartyRSV208;            //Offset 667
//   UINT8    ThirdPartyRSV209;            //Offset 668
//   UINT8    ThirdPartyRSV210;            //Offset 669
//   UINT8    ThirdPartyRSV211;            //Offset 670
//   UINT8    ThirdPartyRSV212;            //Offset 671
//   UINT8    ThirdPartyRSV213;            //Offset 672
//   UINT8    ThirdPartyRSV214;            //Offset 673
//   UINT8    ThirdPartyRSV215;            //Offset 674
//   UINT8    ThirdPartyRSV216;            //Offset 675
//   UINT8    ThirdPartyRSV217;            //Offset 676
//   UINT8    ThirdPartyRSV218;            //Offset 677
//   UINT8    ThirdPartyRSV219;            //Offset 678
//   UINT8    ThirdPartyRSV220;            //Offset 679
// AMD IDS End
//==========================================================================================================

#endif