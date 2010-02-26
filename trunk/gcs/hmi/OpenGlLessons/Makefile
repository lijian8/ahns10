LIBS = -lglut
INCLUDE = include
PROG = mockupAH

$(PROG):		obj/logger.o obj/ATT_draw.o obj/AH_setup.o obj/AH_Utility.o obj/ALT_draw.o obj/imageloader.o obj/main.o
	g++ -Wall -o $(PROG) obj/logger.o obj/ATT_draw.o obj/AH_setup.o obj/AH_Utility.o obj/ALT_draw.o obj/imageloader.o obj/main.o $(LIBS)

obj/main.o:		src/main.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/main.o src/main.cpp

obj/logger.o:		../../reuse/ahns_logger.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/logger.o ../../reuse/ahns_logger.cpp

obj/ATT_draw.o :	src/ATT_draw.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/ATT_draw.o src/ATT_draw.cpp
	
obj/AH_setup.o:		src/AH_setup.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/AH_setup.o src/AH_setup.cpp
	
obj/ALT_draw.o :	src/ALT_draw.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/ALT_draw.o src/ALT_draw.cpp
	
obj/AH_Utility.o:	src/AH_Utility.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/AH_Utility.o src/AH_Utility.cpp
	
obj/imageloader.o:	src/imageloader.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/imageloader.o src/imageloader.cpp



clean:
	rm -f $(PROG) obj/*.o
