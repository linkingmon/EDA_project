# EDA_project
## 說明
1. 跑 drawS.py 可看到最後結果
## 進度
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
