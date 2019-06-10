# EDA_project
## 說明
1. 吃 test_merge.txt 再跑 draw.py 可以在 result 資料夾看到結果
2. 吃 poly_with_hole.txt 再跑 drawM.py 可以在 result2 資料夾看到有hole的多邊形(可以做Split)
## 進度
- (6/10) OpenCase_1.txt M1 成功，C1當掉 [8]
- (6/10) 修正~~[2]~~，跑得很快
- (6/10) 修正 root 前後可能連成一條線(修正完之後每個點一定是"角") [7] 修正~~[7]~~
- (6/10) 修正~~[3]~~，還有把原先插到 out_list 的點刪掉
- (6/10) 兩個L形成一個洞，然後一個大矩形蓋住有BUG [6]
- (6/10) 自己跟自己形成hole沒有寫到 [5]
- (6/10) 完成大Merge
- (6/10) 修正~~[4]~~
- (6/9) Delete intersect不完全 [4]
- (6/9) 修正~~[1]~~
- (6/9) 把 Merge / Clip / Split 寫到 operation.h 裡面
- (6/8) 如果A、B、C、D....、J的交點都找好，結果發現K多邊形把A蓋住，要把A刪掉以外，還要把跟A有交點的多邊形的交點刪掉(很難寫) -> 可以把要刪掉的多邊形交點對面改tran = false [3]
- (6/8) 跑Opencase1.txt發現很沒效率(交點找很久)，可以記錄(min,max)(x,y)，太遠的就不用比較 [2]
- (6/8) 現在merge出來的多邊形可能有一個邊上有很多點，還沒優化，應該把多餘的點刪掉 [1]
- (6/7) 單operation merge完成，可以吃test_merge.txt(可以自己改裡面的多邊形個數、形狀)再跑draw.py，就可以在result資料夾看到結果
