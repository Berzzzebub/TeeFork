# Author and Copyright: Bohdan Turkynewych 2009-2010, <tb0hdan@gmail.com
# Compiles with modified BSD license.
# See GPL.txt
#C:\Users\Zet\Documents\Visual Studio 2008\Projects\Tee\Tee\srcC:\Users\Zet\Documents\Visual Studio 2008\Projects\Tee\Tee\other\sdl\includeC:\Users\Zet\Documents\Visual Studio 2008\Projects\Tee\Tee\src\engine\external\zlibC:\Users\Zet\Documents\Visual Studio 2008\Projects\Tee\Tee\data
####
CFLAGS		= -DCONF_FAMILY_UNIX -g -Wall -O2 -fstack-protector -fstack-protector-all -fno-exceptions
CXXFLAGS	+= $(CFLAGS)
LDFLAGS         += -lpthread -lpng
GCC		= gcc
G++		= g++
# Define some includes
BASEINC		= -I./base
ENGNINC		= -I./engine -I./engine/client -I./engine/server -I./engine/external/pnglite -I./engine/external/wavpack -I./engine/external/zlib
GAMEINC		= -I./game -I./game/client -I./game/client/components -I./game/editor -I./game/generated -I./game/server -I./game/server/entities -I./game/server/gamemodes
MSRVINC		= -I./mastersrv
VRSVINC		= -I./versionsrv
INCLUDES	= -I/usr/include -I./ $(BASEINC) $(ENGNINC) $(GAMEINC) $(MSRVINC) $(VRSVINC)
#
OBJPATH		= ./OBJ
OBJDIR		= $(OBJPATH)
# Objects for fake_server
SYSOBJ		= $(OBJPATH)/system.o
ERGNOBJ		= $(OBJPATH)/e_ringbuffer.o
EENGOBJ		= $(OBJPATH)/e_engine.o
ENETOBJ		= $(OBJPATH)/e_network.o
EHUFOBJ		= $(OBJPATH)/e_huffman.o
EJOBOBJ		= $(OBJPATH)/e_jobs.o
ECONOBJ		= $(OBJPATH)/e_console.o
ECFGOBJ		= $(OBJPATH)/e_config.o
ELRDOBJ		= $(OBJPATH)/e_linereader.o
ENTCOBJ		= $(OBJPATH)/e_network_conn.o
ENTSOBJ		= $(OBJPATH)/e_network_server.o
# Additional variables
COMPILE		= $(INCLUDES) $(CFLAGS)
# Pass these variables to make subprocesses.
export 		CFLAGS CXXFLAGS
FAKEOBJS	= $(SYSOBJ) $(ERGNOBJ) $(EENGOBJ) $(ENETOBJ) $(EHUFOBJ) $(EJOBOBJ) \
		    $(ECONOBJ) $(ECFGOBJ) $(ELRDOBJ) $(ENTCOBJ) $(ENTSOBJ)
#
ECLIOBJ		= $(OBJPATH)/e_network_client.o
#
MSTROBJS	= $(FAKEOBJS) $(ECLIOBJ)
#
SRVOBJS = $(FAKEOBJS)
SRVOBJS += $(OBJDIR)/gs_data.o
SRVOBJS += $(OBJDIR)/player.o
SRVOBJS += $(OBJDIR)/mod.o
SRVOBJS += $(OBJDIR)/dm.o
SRVOBJS += $(OBJDIR)/tdm.o
SRVOBJS += $(OBJDIR)/ctf.o
SRVOBJS += $(OBJDIR)/gamecontroller.o
SRVOBJS += $(OBJDIR)/entity.o
SRVOBJS += $(OBJDIR)/character.o
SRVOBJS += $(OBJDIR)/laser.o
SRVOBJS += $(OBJDIR)/pickup.o
SRVOBJS += $(OBJDIR)/projectile.o
SRVOBJS += $(OBJDIR)/eventhandler.o
SRVOBJS += $(OBJDIR)/hooks.o
SRVOBJS += $(OBJDIR)/gameworld.o
SRVOBJS += $(OBJDIR)/gamecontext.o
SRVOBJS += $(OBJDIR)/g_protocol.o
SRVOBJS += $(OBJDIR)/nethash.o
SRVOBJS += $(OBJDIR)/collision.o
SRVOBJS += $(OBJDIR)/layers.o
SRVOBJS += $(OBJDIR)/gamecore.o
SRVOBJS += $(OBJDIR)/es_register.o
SRVOBJS += $(OBJDIR)/e_keynames.o
SRVOBJS += $(OBJDIR)/e_memheap.o
SRVOBJS += $(OBJDIR)/e_demorec.o
SRVOBJS += $(OBJDIR)/e_snapshot.o
SRVOBJS += $(OBJDIR)/e_packer.o
SRVOBJS += $(OBJDIR)/e_compression.o
SRVOBJS += $(OBJDIR)/e_datafile.o
SRVOBJS += $(OBJDIR)/e_msg.o
SRVOBJS += $(OBJDIR)/e_map.o

#
.PHONY:         all clean

all: packetgen crapnet dilate fake_server master_server version_server

$(SYSOBJ): 
	mkdir -p $(OBJPATH)
	$(GCC) -c $(COMPILE) ./base/system.c -o $@

$(ERGNOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_ringbuffer.c -o $@	

$(EENGOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_engine.c -o $@	

$(ENETOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_network.c -o $@		    		

$(EHUFOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_huffman.c -o $@	

$(EJOBOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_jobs.c -o $@	

$(ECONOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_console.c -o $@	

$(ECFGOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_config.c -o $@	

$(ELRDOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_linereader.c -o $@	

$(ENTCOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_network_conn.c -o $@

$(ENTSOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_network_server.c -o $@	

$(ECLIOBJ): $(SYSOBJ)
	$(GCC) -c $(COMPILE) ./engine/e_network_client.c -o $@		

packetgen: $(SYSOBJ)
	$(GCC) $(COMPILE) $(LDFLAGS) ./tools/packetgen.c -o ./$@ $(SYSOBJ)

crapnet: $(SYSOBJ)
	$(G++) $(COMPILE) $(LDFLAGS) ./tools/crapnet.cpp -o ./$@ $(SYSOBJ)

dilate: $(SYSOBJ)
	$(GCC) $(COMPILE) $(LDFLAGS) ./tools/dilate.c	-o ./$@ $(SYSOBJ)

fake_server: $(FAKEOBJS)
	$(GCC) $(COMPILE) $(LDFLAGS) ./tools/fake_server.c -o ./$@ $(FAKEOBJS)

master_server: $(MSTROBJS)
	$(G++) $(COMPILE) $(LDFLAGS) ./mastersrv/mastersrv.cpp -o ./$@ $(MSTROBJS)

version_server: $(MSTROBJS)
	$(G++) $(COMPILE) $(LDFLAGS) ./versionsrv/versionsrv.cpp -o ./$@ $(MSTROBJS)	


teeworlds_server: $(FAKEOBJS)
	#$(G++) $(COMPILE) ./game/generated/gs_data.cpp -o $(OBJDIR)/gs_data.o
	$(G++) $(COMPILE) ./game/server/gamecontext.cpp -o $(OBJDIR)/gamecontext.o $(SYSOBJ)
	# $(G++) $(COMPILE) -lpthread ./game/server/gamecontroller.cpp -I./game -o $(OBJDIR)/gamecontroller.o $(SYSOBJ)
	#$(G++) $(COMPILE) -lpthread -I./game/server ./game/server/gameworld.cpp -o $(OBJDIR)/gameworld.o $(SYSOBJ)
	#$(G++) $(COMPILE) -I./game/server/gamemodes ./game/server/player.cpp -o $(OBJDIR)/player.o $(SYSOBJ) $(OBJDIR)/
#$(G++) -c $(COMPILE) -I./game/server/gamemodes ./game/server/gamemodes/mod.cpp -o $(OBJDIR)/mod.o
#$(G++) -c $(COMPILE) -I./game/server/gamemodes ./game/server/gamemodes/dm.cpp -o $(OBJDIR)/dm.o
#$(G++) -c $(COMPILE) -I./game/server/gamemodes ./game/server/gamemodes/tdm.cpp -o $(OBJDIR)/tdm.o
#$(G++) -c $(COMPILE) -I./game/server/gamemodes -I./game/server -I./game ./game/server/gamemodes/ctf.cpp -o $(OBJDIR)/ctf.o

#$(G++) -c $(COMPILE) -I./game/server ./game/server/entity.cpp -o $(OBJDIR)/entity.o
#$(G++) -c $(COMPILE) -I./game/server/entities ./game/server/entities/character.cpp -o $(OBJDIR)/character.o
#$(G++) -c $(COMPILE) -I./game/server/entities ./game/server/entities/laser.cpp -o $(OBJDIR)/laser.o
#$(G++) -c $(COMPILE) -I./game/server/entities ./game/server/entities/pickup.cpp -o $(OBJDIR)/pickup.o
#$(G++) -c $(COMPILE) -I./game/server/entities ./game/server/entities/projectile.cpp -o $(OBJDIR)/projectile.o
#$(G++) -c $(COMPILE) -I./game/server/ ./game/server/eventhandler.cpp -o $(OBJDIR)/eventhandler.o
#$(G++) -c $(COMPILE) -I./game/server ./game/server/hooks.cpp -o $(OBJDIR)/hooks.o


#$(G++) -c $(COMPILE) -I./game/generated ./game/generated/g_protocol.cpp -o $(OBJDIR)/g_protocol.o
#$(GCC) -c $(COMPILE) -I./game/generated ./game/generated/nethash.c -o $(OBJDIR)/nethash.o
#$(G++) -c $(COMPILE) -I./game ./game/collision.cpp -o $(OBJDIR)/collision.o
#$(G++) -c $(COMPILE) -I./game ./game/layers.cpp -o $(OBJDIR)/layers.o
#$(G++) -c $(COMPILE) -I./game ./game/gamecore.cpp -o $(OBJDIR)/gamecore.o
#$(GCC) -c $(COMPILE) ./engine/server/es_register.c -o $(OBJDIR)/es_register.o
#$(GCC) -c $(COMPILE) ./engine/e_keynames.c -o $(OBJDIR)/e_keynames.o
#$(GCC) -c $(COMPILE) ./engine/e_memheap.c -o $(OBJDIR)/e_memheap.o
#$(GCC) -c $(COMPILE) ./engine/e_demorec.c -o $(OBJDIR)/e_demorec.o
#$(GCC) -c $(COMPILE) ./engine/e_snapshot.c -o $(OBJDIR)/e_snapshot.o
#$(GCC) -c $(COMPILE) ./engine/e_packer.c   -o $(OBJDIR)/e_packer.o
#$(GCC) -c $(COMPILE) ./engine/e_compression.c -o $(OBJDIR)/e_compression.o
#$(GCC) -c $(COMPILE) ./engine/e_datafile.c    -o $(OBJDIR)/e_datafile.o
#$(GCC) -c $(COMPILE) ./engine/e_msg.c         -o $(OBJDIR)/e_msg.o
#$(GCC) -c $(COMPILE) ./engine/e_map.c         -o $(OBJDIR)/e_map.o
# Server source
#$(GCC) $(COMPILE) $(LDFLAGS) ./engine/server/es_server.c -o ./$@ $(SRVOBJS)

clean:  
	rm -rf $(OBJDIR) packetgen crapnet dilate fake_server master_server version_server

