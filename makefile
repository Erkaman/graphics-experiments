all:
	make -C build/ game && cd samples/game/ && ../../build/samples/game/game ../..
