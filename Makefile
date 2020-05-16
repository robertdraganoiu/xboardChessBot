LIBS = lot_bot.h utils.h engine_driver.h board.h pieces.h
FILES = main.cpp lot_bot.cpp engine_driver.cpp board.cpp pieces.cpp

build: $(LIBS) $(FILES)
	g++ $(FILES) -o lot_bot -O3

run: lot_bot
	./lot_bot

xboard: lot_bot
	xboard -fcp "./lot_bot" -debug

check: lot_bot
	xboard -fcp "make run" -scp fairymax -secondInitString "new\nrandom\nsd 4\n" -tc 5 -inc 2 -mg 10 -autoCallFlag true -sgf partide.txt  -reuseFirst false -debug




xboard_bot_as_white: lot_bot
	xboard -fcp "./lot_bot" -scp fairymax -secondInitString "new\nrandom\nsd 4\n" -mg 1 -debug

xboard_bot_as_black: lot_bot
	xboard -fcp fairymax -secondInitString "new\nrandom\nsd 4\n" -scp "./lot_bot" -mg 1 -debug

clean: lot_bot
	rm lot_bot
