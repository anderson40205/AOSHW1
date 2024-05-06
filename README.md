先介紹一下四種 page replacement algorithm運作原理，以及我的實作方式
1.	First in first out algorithm(FIFO)：
先進先出，演算法中最容易實作且現行OS大多都採用的方式，最先放入的會是frame裡最早被替換的。
實作是以queue來設計，因為這個資料結構本身就有先進先出的特性。
其他輔助的資料結構有dirty bit陣列和是否在記憶體(isInMemory)的陣列(所有演算法都有)。
2.	Optimal algorithm(OPT)：
未來長期不會參考到的page，作為victim page。Page fault ratio一定是所有演算法中最低的，不會遭遇Belady's Anomaly。但理論上是無法被實作的，因為要看的是未來，本次作業只是先有了資料能看未來才實作的出來。資料結構跟FIFO差不多，queue變成使用vector以及其他輔助資訊的陣列，若未來的值都沒出現，則替換第一個frame。
3.	Enhance-Second Chance algorithm(ESC)：
以<Reference Bit, Modification Bit>配對值作為挑選victim page的依據，值最小的page視為victim page，若全部page具相同值，則替換第一個index。
<0, 0>：最近沒被行程使用，也沒被行程修改
<0, 1>：表示最近沒有被行程使用，但有被修改過，須先寫回磁碟後，才可進行替換
<1, 0>：表示最近曾被行程使用，但是沒被修改過，由於可能再次被使用
<1, 1>：表示最近曾被行程使用，也被修改過，所以要寫回磁碟中
實作中使用的資料結構跟前面差不多，只是多了reference bit陣列，邏輯是只要全部reference bit都變1，ESC_checkReferenceBits這個函數就會把全部reference bit都變0，中斷額外加1。所以實際上我的判斷只有在比<0,0>和<0,1>而已
4.	My algorithm(MyAlgo)：
我所設計的page replacement演算法是用區段統計的方式，統計1~600這些reference string的出現次數，將來victim page會替換掉出現次數最少的，如果全部的出現次數都一樣，就像OPT一樣，替換掉第一個frame的page。而這些統計的出現次數每1000次會歸0重新開始，會這樣做是因為要給其他人一些機會，因為有的ref str累積太高，但他後面根本不常出現，卻不會被替換掉，這時候常出現的資料反而會一直被替換掉，就會造成部分區段的不公平，資料結構跟OPT差不多，但多了統計1~600出現次數的陣列。

三種測試資料:
1.	Random pick：
reference string的選取方式是18萬次每次都random一個值(範圍1~600)。
2.	Locality pick：
先定好要貢獻的資料長度，我是random 500~1000，一直貢獻直到18萬筆資料為止，接著在15~20個左右數字集合的資料間(作業規定)random反覆挑選，在600個數字中random一個head，範圍: 1到580，我取580的原因是因為萬一取到20，580+20也不會超過600。
3.	My pick：
我的作法是先用Random pick的方法，18萬筆資料產生之後，隨機1~5步個frame，塞我製造的50個特殊數字Find50SpecificNumber這個函數，也就是這50個數字會穿插在我的隨機數字中，而這50個數字都不一樣，會照順序穿插，50個不一樣的數用完就在輪一次，直到18萬筆資料都塞得差不多。所以對於我的演算法非常有利，因為我用統計的方式可以讓這幾個常出現的數字免於被替換掉的風險。
