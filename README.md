# EDA_project
## 使用說明
0. our program is running on ubuntu 18.04<br>
1. run "bash setup.sh" to install the correct gcc version, unzip testcasesm unzip Checkers and compile our source code<br>
2. run "./a.out < $input_filename > $output_filename" to generate the result. Ex. "./a.out < OpenCase_1.txt > res_1.txt" will input OpenCase_1.txt and output the result to res_1.txt<br>
3. run "./CHECKER $input_filename $OpenCase_num" to check if the result is correct on OpenCase1 and 2. The CHECKER file is provided by ICCAD contest. Ex. "./CHECKER res_1.txt 1" will input the result and check if the answer is consistent with the correct answer of OpenCase_1.<br>
4. Note that OpenCase2_txt need to run 3.5 hours to get the result, we save the reult in the "final_result.zip" folder.<br><br>
## 包含檔案
1. Readme file:<br> 
Readme.md<br> 
2. Final presentation powerpoint:<br> 
Algorithm_final.ppt<br>
3. Final report:<br>
Rectilinear Polygon Operators for Phsical Design<br>
4. Reference paper:<br> 
reference/Graph-Theoretic Solutions.pdf<br> 
         /Polygon Clipping and Filling.pdf<br> 
5. Source code:<br> 
main.cpp<br> 
glob_func.h<br> 
HopcroftKarft.h<br> 
interval.h<br> 
littlemerge.h<br> 
operation.h<br> 
point.h<br> 
SplitMgr.h<br> 
TimeMgr.h<br> 
6. Shell script:<br>
setup.sh<br>
7. Python file for plotting result:<br>
drawS.py<br><br>

## 進度
- (6/23) 完成 OpenCase_1 和 OpenCase_2 並且都通過官方 Checker
- (6/23) 修正如果有很扁的多邊形(面積0，就是往右再往左)，直接刪掉
- (6/22) 完成SO，Opencase_1可以切成446塊 
- (6/22) Merge和Clip都要在Walk完後把非cycle的部分去掉。
- (6/20) 應該要開 root class。[16]
- (6/20) 新增 TimeMgr class可用來計算時間。
- (6/20) 就算是座標一樣，面積算起來也可能部一樣...，換方法，用outside poly，當兩個互相包含的時候就把min_hole_area和min_poly_area設為DBL_MIN。[15] ~~[15]~~
- (6/20) 修改bug，兩個in的情況(對被merge的人來說in一定會判斷正確，因為放進來的是多邊形，但是merge人的可能在判斷走到邊上的in/out時把洞誤判成多邊形)[14] ~~[14]~~
- (6/19) (加速)修改check list，在point class加入新的變數iscolored，只要其中有交點被塗到，就直接到該多邊形的root把iscolored改為true，這樣在check的時候只要看root的iscolored就好，不需要搜尋整個多邊形。
- (6/19) (加速)修改insert intersect，直接在littlemerge裡面存有插入焦點的線段的頭，直接對頭進行insert就好。
- (6/19) 自己和自己merge還沒寫到。[5]
- (6/19) 修正bug，merge多邊形如果再洞裡面且面積相同，要把洞刪掉。(clip相反)
- (6/18) karp algorithm完成。
- (6/18) OpenCase_2 bad_alloc
- (6/18) OpenCase_1全部完成，總共449塊矩形。 ~~[13]~~ ~~[11]~~
- (6/17) OpenCase_1.txt中的C2還有部分bug，洞沒有何在一起。
- (6/17) 修改走過的部分不要走(clip會有很多這種情況)，如果是繞一圈就咖掉，如果還沒繞一圈就走到走過的就不要算(還有bug[13])
- (6/17) OpenCase_1.txt到C1為止都正確
- (6/17) 修改小bug，還有更正clip的演算法。用clip那塊的in/out來看，in一定要走，out一定要換到對面。
- (6/16) Merge完成，可以把 OpenCase_1.txt 或 OpenCase_2.txt 或2中的C1、C2改成M3、M4來測試
- (6/16) 修改交點sorting順序，如果座標相同，in的擺在後面。因為out不會影響繼續在該路徑行走，in的對面有可能是out必須要換。如果沒有這樣sort，會少merge多邊形。
- (6/16) ~~[12]~~ 修改一個點走出去再走回來轉彎(一條線)在list construct後會變一個點。用assert確定正確性。
- (6/16) List construct[12]
- (6/16) Clip會Clip出一條線??[11]，merge在Opencase2也會，不能有線(不能Split)
- (6/16) OpenCase_1.txt的merge完成，都沒問題(Clip還沒)
- (6/15) 修改要merge或clip的多邊形被包含的情況：被包含時min_poly_area(min_hole_are)存最小包含該多邊形的多邊形(洞)面積，如果是merge若min_poly_area小於min_hole_area就要刪掉，否則留下；clip則相反。
- (6/15) 修改delete多邊形後，delete交點的方法：再new intersect時把頂點的指標存到intersect_list最後去頂點pop_back
- (6/15) 修正root_list中會有交點的問題
- (6/15) 新演算法：遇到交點只走out的交點(對面是out就換，自己是out就繼續走(一直保持最外的性質))
- (6/15) 新演算法：交點找法一樣。但若有兩個in或兩個out就不要找交點(參考check_point function) ~~[10]~~
- (6/14) Clip有bug：(1)tran會有錯誤 (2)in交點非常多(可能要重新考慮什麼是in交點) [10]
- (6/14) 不管是Merge還是Clip都是merge，Merge從out開始，Clip從in開始；另外如果有包含關係不太一樣要特別處理
- (6/14) 在讀到Clip時，直接把list存成順時針
- (6/14) 修改讀檔最後一個點可能不會給的錯誤[9] ~~[9]~~
- (6/13) SplitV/H完成
- (6/13) 重新設計演匴法 ~~[6]~~，利用左邊是裡面的概念+特殊狀況判斷
- (6/10) 修正 ~~[8]~~，(verti和dir只有if沒有else)
- (6/10) OpenCase_1.txt M1 成功，C1當掉 [8]
- (6/10) 修正 ~~[2]~~，跑得很快
- (6/10) 修正 root 前後可能連成一條線(修正完之後每個點一定是"角") [7] 修正 ~~[7]~~
- (6/10) 修正 ~~[3]~~，還有把原先插到 out_list 的點刪掉
- (6/10) 兩個L形成一個洞，然後一個大矩形蓋住有BUG [6]
- (6/10) 自己跟自己形成hole沒有寫到 [5]
- (6/10) 完成大Merge
- (6/10) 修正 ~~[4]~~
- (6/9) Delete intersect不完全 [4]
- (6/9) 修正 ~~[1]~~
- (6/9) 把 Merge / Clip / Split 寫到 operation.h 裡面
- (6/8) 如果A、B、C、D....、J的交點都找好，結果發現K多邊形把A蓋住，要把A刪掉以外，還要把跟A有交點的多邊形的交點刪掉(很難寫) -> 可以把要刪掉的多邊形交點對面改tran = false [3]
- (6/8) 跑Opencase1.txt發現很沒效率(交點找很久)，可以記錄(min,max)(x,y)，太遠的就不用比較 [2]
- (6/8) 現在merge出來的多邊形可能有一個邊上有很多點，還沒優化，應該把多餘的點刪掉 [1]
- (6/7) 單operation merge完成，可以吃test_merge.txt(可以自己改裡面的多邊形個數、形狀)再跑draw.py，就可以在result資料夾看到結果

