#ifndef __MODBUS_DEF__
#define __MODBUS_DEF__

#define DRIVER_DATA_DIR "AW_DATA_DIR"
#define	ADAPTOR_NAME	"modbus"

// Overwrite the max number of template variable
#undef MAX_VAR
#define	MAX_VAR	        1024

#define MAX_SSR_PER_RTR 10240

#define CHECK_IS_OBIX(dst) \
    	if	(!dst) return; if (XoIsObix(dst) <= 0) {RTL_TRDBG(0,"ERROR not an obix object\n");return;}

#define	DIA_KO()	(DiaIpuOk == 0 || DiaNetOk == 0)

#define CONTINUE_IF_NULL(x) if((x) == NULL) continue;

// rtl_trace levels, must be move in rtlbase...
#define TRACE_ERROR     0
#define TRACE_API       1
#define TRACE_INFO      2
#define TRACE_IMPL      5
#define TRACE_DETAIL    6
#define TRACE_FULL      10

// MODBUS internal events
#define	IM_DEF			        1000
#define	IM_TIMER_GEN		    1001
#define IM_TIMER_GEN_V		    10000	// ms

// Max of t_cov struct
#define	CZ_MAX  100

/*
 *  Easy Var init for t_var struct
 *  must be move in drvcommon
 */
#define VAR_INIT_EMPTY(x) \
    {.v_name = #x, .v_default = "", .v_fct = NULL, .v_value = NULL}

#define VAR_INIT(x, y) \
    {.v_name = #x, .v_default = y, .v_fct = NULL, .v_value = NULL}

#define VAR_FUNCTION(x, y) \
    {.v_name = #x, .v_default = NULL, .v_fct = y, .v_value = NULL}

#define SetVarFromXo(x,y,z) \
    tmp = (char *) XoNmGetAttr(y, z, 0); \
    if(tmp == NULL) {SetVar(x, "");} else {SetVar(x, tmp);}
    
/*
 *  Some import from Linux Kernel
 */
#define BUILD_BUG_ON(condition) \
    ((void)sizeof(char[1 - 2*!!(condition)]))

#define get_unaligned(ptr) \
    ({ __typeof__(*(ptr)) __tmp; memcpy(&__tmp, (ptr), sizeof(*(ptr))); __tmp; })

/*
 *  GCC Tricks
 */
#define NOT_USED(x) ((void)(x))
#define likely(x)    __builtin_expect (!!(x), 1)
#define unlikely(x)  __builtin_expect (!!(x), 0)
#define xstr(s) str(s)
#define str(s) #s

#endif /* __MODBUS_DEF__ */
