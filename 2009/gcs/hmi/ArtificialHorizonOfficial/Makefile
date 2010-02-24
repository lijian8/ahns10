LIBS = -lglut
INCLUDE = include
PROG = OfficialAH

$(PROG):		obj/logger.o obj/AH_draw.o obj/AH_setup.o obj/AH_Utility.o obj/ATT_Element.o obj/ATT_text.o obj/ALT_text.o obj/main.o
	g++ -Wall -o $(PROG) obj/logger.o obj/AH_draw.o obj/AH_setup.o obj/AH_Utility.o obj/ATT_Element.o obj/ATT_text.o obj/ALT_text.o obj/main.o $(LIBS)

obj/main.o:		src/main.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/main.o src/main.cpp

obj/logger.o:		../../reuse/ahns_logger.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/logger.o ../../reuse/ahns_logger.cpp

obj/AH_setup.o:		src/AH_setup.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/AH_setup.o src/AH_setup.cpp
	
obj/AH_draw.o:		src/AH_draw.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/AH_draw.o src/AH_draw.cpp
	
obj/AH_Utility.o:	src/AH_Utility.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/AH_Utility.o src/AH_Utility.cpp
	
obj/ATT_Element.o:	src/ATT_Element.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/ATT_Element.o src/ATT_Element.cpp
	
obj/ATT_text.o:		src/ATT_text.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/ATT_text.o src/ATT_text.cpp
	
obj/ALT_text.o:		src/ALT_text.cpp
	g++ -Wall -c -I $(INCLUDE) -o obj/ALT_text.o src/ALT_text.cpp



clean:
	rm -f $(PROG) obj/*.o *.log
