#ifndef OPERMGR_H
#define OPERMGR_H
#include "point.h"
#include "operation.h"
#include "MergeMgr.h"
#include "ClipMgr.h"
#include "SplitMgr.h"
class OperMgr
{
public:
    OperMgr(vector<string> &operations_t, map<string, operation> mapping_t);
    ~OperMgr();
    void do_operation();

private:
    vector<string> operations;
    map<string, operation> mapping;
    MergeMgr *mergemgr;
    ClipMgr *clipmgr;
    SplitMgr *splitmgr;
    vector<point *> total;
};
OperMgr::OperMgr(vector<string> &operations_t, map<string, operation> mapping_t) : operations(operations_t), mapping(mapping_t)
{
    mergemgr = new MergeMgr();
    clipmgr = new ClipMgr();
    splitmgr = new SplitMgr();
}
OperMgr::~OperMgr()
{
}
void OperMgr::do_operation()
{
    for (unsigned int i = 0; i < operations.size(); ++i)
    {
        if (operations[i][0] == 'M')
        {
            operation &oper = mapping[operations[i]];
            mergemgr->merge(total, oper.root_list);
        }
        else if (operations[i][0] == 'C')
        {
            operation &oper = mapping[operations[i]];
            clipmgr->clip(total, oper.root_list);
        }
        else
        { // operations[i] = "SV" "SH" "SO"
            if (operations[i][1] == 'V')
                splitmgr->splitV(total);
            else if (operations[i][1] == 'H')
                splitmgr->splitH(total);
            else
                splitmgr->splitO(total);
        }
    }
}
#endif