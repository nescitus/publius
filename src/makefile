CXX=g++

SRCS= \
	badcapture.cpp \
	bitboard.cpp \
	bitgen.cpp \
	color.cpp  \
	diagnostics.cpp \
	endgame.cpp \
	eval.cpp \
	evaldata.cpp \
	evalhash.cpp \
	gen.cpp \
	hashkeys.cpp \
	history.cpp \
	list.cpp \
	lmr.cpp \
	legality.cpp \
	main.cpp \
	mask.cpp \
	move.cpp \
	movedo.cpp \	
	moveundo.cpp \
	piece.cpp \
	position.cpp \
	position_getters.cpp \
	pst.cpp \
	pv.cpp \
	quiesce.cpp \
	root.cpp \
	search.cpp \
	square.cpp \
	timer.cpp \
	trans.cpp \
	uci.cpp \
    util.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = publius

all:	$(TARGET)

CXXFLAGS=-Wall

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET):	$(SRCS)
		g++ -o publius $(SRCS)

clean:	.
	@rm -f *.o $TARGET
