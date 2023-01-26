#I'm not sure is c or c++ in future
all: main plot
#看main後面除了main.c還會跟誰合併 ex:後面接了tool.o，
#你就要在下面寫tool.o:tool.c gcc -c tool.c
main: main.cpp
	g++ main.cpp -o main
plot: plot.c
	gcc plot.c pbPlots.c supportLib.c -lm
#要echo東西請放main: main.c底下
#gcc -c main.c -c代表把.c檔變成.o檔 用-c參數表示gcc不會鏈接函式庫，
#但會對每一個輸入的檔案產生目的碼，也就是.o檔
clean:
	rm *.o main
#清掉所有.o檔和main執行檔
#-lm -Wall -g 可以存到CFLAGS
#-Wall表示編譯器會跳出警告訊息
#使用-g選項，可以允許gcc在目的檔和執行檔中包含符號表和一些源程式碼的資訊。