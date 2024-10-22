/// <PostCodePrefix> 0xB0003000

#define BiosAslTpbasevalue                          0xB0000000

////GPP1MS
#define MsGpp1Ps0Method                              0x3101
#define MsGpp1Ps3Method                              0x3103
#define MsGpp1S0wMethod                              0x3104

#define MsGpp1Pr0StaMethod                           0x3105
#define MsGpp1Pr0OnMethod                            0x3106
#define MsGpp1Pr0OffMethod                           0x3108

#define MsGpp1Pr3StaMethod                           0x310A
#define MsGpp1Pr3OnMethod                            0x310B
#define MsGpp1Pr3OffMethod                           0x310D

//GPP1->WLAN
#define MsWlanPs0Method                              0x3111
#define MsWlanPs3Method                              0x3113
#define MsWlanS0wMethod                              0x3114
	
#define MsWlanPr0StaMethod                           0x3115
#define MsWlanPr0OnEntry                             0x3116
#define MsWlanPr0OnExit                              0x3117
#define MsWlanPr0OffEntry                            0x3118
#define MsWlanPr0OffExit                             0x3119
	
#define MsWlanPr3StaMethod                           0x311A
#define MsWlanPr3OnMethod                            0x311B
#define MsWlanPr3OffMethod                           0x311D

//GPP1->DT
#define MsDtPs0Method                                0x3121
#define MsDtPs3Method                                0x3123
#define MsDtS0wMethod                                0x3124
		
#define MsDtPr0StaMethod                             0x3125
#define MsDtPr0OnEntry                               0x3126
#define MsDtPr0OnExit                                0x3127
#define MsDtPr0OffEntry                              0x3128
#define MsDtPr0OffExit                               0x3129
		
#define MsDtPr3StaMethod                             0x312A
#define MsDtPr3OnMethod                              0x312B
#define MsDtPr3OffMethod                             0x312D

////GPP2MS
#define MsGpp2Ps0Method                              0x3201
#define MsGpp2Ps3Method                              0x3203
#define MsGpp2S0wMethod                              0x3204

#define MsGpp2Pr0StaMethod                           0x3205
#define MsGpp2Pr0OnMethod                            0x3206
#define MsGpp2Pr0OffMethod                           0x3208

#define MsGpp2Pr3StaMethod                           0x320A
#define MsGpp2Pr3OnMethod                            0x320B
#define MsGpp2Pr3OffMethod                           0x320D

//GPP2->OnboardLan
#define MsNicPs0Method                               0x3211
#define MsNicPs3Method                               0x3213
#define MsNicS0wMethod                               0x3214
	
#define MsNicPr0StaMethod                            0x3215
#define MsNicPr0OnEntry                              0x3216
#define MsNicPr0OnExit                               0x3217
#define MsNicPr0OffEntry                             0x3218
#define MsNicPr0OffExit                              0x3219
	
#define MsNicPr3StaMethod                            0x321A
#define MsNicPr3OnMethod                             0x321B
#define MsNicPr3OffMethod                            0x321D

////GPP3MS
#define MsGpp3Ps0Method                              0x3301
#define MsGpp3Ps3Method                              0x3303
#define MsGpp3S0wMethod                              0x3304

#define MsGpp3Pr0StaMethod                           0x3305
#define MsGpp3Pr0OnMethod                            0x3306
#define MsGpp3Pr0OffMethod                           0x3308

#define MsGpp3Pr3StaMethod                           0x330A
#define MsGpp3Pr3OnMethod                            0x330B
#define MsGpp3Pr3OffMethod                           0x330D

//GPP3->Wwan
#define MsWwanPs0Method                              0x3311
#define MsWwanPs3Method                              0x3313
#define MsWwanS0wMethod                              0x3314
	
#define MsWwanPr0StaMethod                           0x3315
#define MsWwanPr0OnEntry                             0x3316
#define MsWwanPr0OnExit                              0x3317
#define MsWwanPr0OffEntry                            0x3318
#define MsWwanPr0OffExit                             0x3319
	
#define MsWwanPr3StaMethod                           0x331A
#define MsWwanPr3OnMethod                            0x331B
#define MsWwanPr3OffMethod                           0x331D

////GPP6MS
#define MsGpp6Ps0Method                              0x3601
#define MsGpp6Ps3Method                              0x3603
#define MsGpp6S0wMethod                              0x3604

#define MsGpp6Pr0StaMethod                           0x3605
#define MsGpp6Pr0OnMethod                            0x3606
#define MsGpp6Pr0OffMethod                           0x3608

#define MsGpp6Pr3StaMethod                           0x360A
#define MsGpp6Pr3OnMethod                            0x360B
#define MsGpp6Pr3OffMethod                           0x360D

//GPP6->NVME SSD
#define MsNvmePs0Method                              0x3611
#define MsNvmePs3Method                              0x3613
#define MsNvmeS0wMethod                              0x3614
	
#define MsNvmePr0StaMethod                           0x3615
#define MsNvmePr0OnEntry                             0x3616
#define MsNvmePr0OnExit                              0x3617
#define MsNvmePr0OffEntry                            0x3618
#define MsNvmePr0OffExit                             0x3619
	
#define MsNvmePr3StaMethod                           0x361A
#define MsNvmePr3OnMethod                            0x361B
#define MsNvmePr3OffMethod                           0x361D

//XHCI0
#define MsXhci0Ps0Method                              0x3C01
#define MsXhci0Ps3Method                              0x3C03
#define MsXhci0S0wMethod                              0x3C04

#define MsXhci0Pr0StaMethod                           0x3C05
#define MsXhci0Pr0OnMethod                            0x3C06
#define MsXhci0Pr0OffMethod                           0x3C08

#define MsXhci0Pr3StaMethod                           0x3C0A
#define MsXhci0Pr3OnMethod                            0x3C0B
#define MsXhci0Pr3OffMethod                           0x3C0D

//XHCI1
#define MsXhci1Ps0Method                              0x3D01
#define MsXhci1Ps3Method                              0x3D03
#define MsXhci1S0wMethod                              0x3D04

#define MsXhci1Pr0StaMethod                           0x3D05
#define MsXhci1Pr0OnMethod                            0x3D06
#define MsXhci1Pr0OffMethod                           0x3D08

#define MsXhci1Pr3StaMethod                           0x3D0A
#define MsXhci1Pr3OnMethod                            0x3D0B
#define MsXhci1Pr3OffMethod                           0x3D0D


