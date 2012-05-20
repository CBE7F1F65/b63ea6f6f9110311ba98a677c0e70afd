#pragma once

#define COMMPARAMMAX	16
#define CSI_TERMINAL	-1
#define CSI_PAUSE		-2
#define CSI_RESUME		-3

enum{
	CSP_PAN_OFFSET_XY = 0,
};

enum{
	CSI_PAN_INIT = 0,
	CSI_PAN_READY,
};

enum{
	CSP_ZOOMIN_B_XY = 0,
	CSP_ZOOMIN_E_XY,
};

enum{
	CSP_DOZOOM_C_XY_SCALE = 0,
};
