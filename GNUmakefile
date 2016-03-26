projects := ochi ochi_dyn

ochi_cfg := release debug

ochi_cflags := -DZLX_STATIC -DHBS_STATIC -DACX1_STATIC -DTWX_STATIC
ochi_csrc := app.c
# ldflags (1: prj, 2: prod, 3: cfg, 4: bld)
ochi_ldflags = -static -ltwx$($3_sfx) -lacx1 -lhbs$($3_sfx) -lzlx$($3_sfx) $(if $(findstring -windows,$($4_target)),-mconsole -municode,-lpthread)
ochi_ldep = $(call prod_path,zlx,slib,$3,$4) $(call prod_path,hbs,slib,$3,$4) $(call prod_path,acx1,slib,release,$4) $(call prod_path,twx,slib,$3,$4)

ochi_dyn_cfg := $(ochi_cfg)
ochi_dyn_cflags :=
ochi_dyn_csrc := $(ochi_csrc)
ochi_dyn_ldflags = $(filter-out -static,$(ochi_ldflags))
ochi_dyn_ldep = $(call prod_path,zlx,dlib,$3,$4) $(call prod_path,hbs,dlib,$3,$4) $(call prod_path,acx1,dlib,release,$4) $(call prod_path,twx,dlib,$3,$4)

include icobld.mk

