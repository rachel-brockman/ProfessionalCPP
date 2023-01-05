#include "Node.h"
#include "parser.hpp"
#include <iostream>

void CodeContext::AddToIntervalsMap(std::string virtualRegister)
{
    std::string stringRegister = virtualRegister.substr(1);
    int intRegister = std::stoi(stringRegister);
    if(mIntervals.find(intRegister) == mIntervals.end()){
        mIntervals.insert(std::pair<int, std::vector<int>>(intRegister, {}));
    }
    mIntervals[intRegister].emplace_back(mOps.size()-1);
}

void CodeContext::AllocateRegisters()
{
    for(int i = 0; i<mOps.size(); i++){
        for(int j = 0; j<mIntervals.size(); j++){
            if(i == mIntervals[j][0]){
                std::string firstAvailable = GetFirstAvailableRegister();
                mRegisterPairs.insert(std::pair<int, std::string>(j, firstAvailable));
            }
            if(i == mIntervals[j].back()){
                std::string registerName = mRegisterPairs[j];
                mRegisterAvailability[registerName] = true;
            }
        }
    }
}

std::string CodeContext::GetFirstAvailableRegister()
{
    std::vector<std::string> trueRegisters;
    trueRegisters.clear();
    for(auto reg: mRegisterAvailability){
        if(reg.second){
            trueRegisters.emplace_back(reg.first);
        }
    }
    mRegisterAvailability[trueRegisters[0]] = false;
    return trueRegisters[0];
}


void NBlock::CodeGen(CodeContext& context)
{
    for(int i = 0; i < mStatements.size(); i++){
        mStatements[i]->CodeGen(context);
    }
}

void NData::CodeGen(CodeContext& context)
{
    for(int i = 0; i < mDecls.size(); i++){
        mDecls[i]->CodeGen(context);
    }
}

void NProgram::CodeGen(CodeContext& context)
{
    mData->CodeGen(context);
    mMain->CodeGen(context);
    std::vector<std::string> emptyVec = {""};
    std::pair<std::string, std::vector<std::string>> pairToAdd = std::make_pair("exit", emptyVec);
    context.mOps.emplace_back(pairToAdd);
}

void NNumeric::CodeGen(CodeContext& context)
{
}

void NVarDecl::CodeGen(CodeContext& context)
{
    std::vector<std::string> regVec = {"r0"};
    std::pair<std::string, std::vector<std::string>> pairToAdd = std::make_pair("push", regVec);
    context.mOps.emplace_back(pairToAdd);
    context.mStackIndexMatches.insert(std::pair<std::string, int>(mName, context.mStackIndex));
    ++context.mStackIndex;
}

void NArrayDecl::CodeGen(CodeContext& context)
{
    std::vector<std::string> regVec = {"r0"};
    std::pair<std::string, std::vector<std::string>> pairToAdd = std::make_pair("push", regVec);
    for(int i = 0; i<(mSize->GetValue()); i++){
        context.mOps.emplace_back(pairToAdd);
    }
    context.mStackIndexMatches.insert(std::pair<std::string, int>(mName, context.mStackIndex));
    ++context.mStackIndex;
}

void NNumericExpr::CodeGen(CodeContext& context)
{
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    mResultRegister = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    std::vector<std::string> regVec = {virtualReg, std::to_string(mNumeric->GetValue())};
    std::pair<std::string, std::vector<std::string>> pairToAdd = std::make_pair("movi", regVec);
    context.mOps.emplace_back(pairToAdd);
    
    context.AddToIntervalsMap(virtualReg);
    std::cout << "";
}

void NVarExpr::CodeGen(CodeContext& context)
{
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    mResultRegister = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    int stackIndex = context.mStackIndexMatches[mName];
    std::vector<std::string> regVec = {virtualReg, std::to_string(stackIndex)};
    std::pair<std::string, std::vector<std::string>> pairToAdd = std::make_pair("loadi", regVec);
    context.mOps.emplace_back(pairToAdd);

    context.AddToIntervalsMap(virtualReg);
    std::cout << "";
}

void NBinaryExpr::CodeGen(CodeContext& context)
{
    mLhs->CodeGen(context);
    mRhs->CodeGen(context);
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    mResultRegister = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    std::vector<std::string> regVec = {virtualReg, mLhs->GetResultRegister(), mRhs->GetResultRegister()};
    std::pair<std::string, std::vector<std::string>> pairToAdd;
    switch (mType)
    {
    case TADD:
        pairToAdd = std::make_pair("add", regVec);
        break;
    case TSUB:
        pairToAdd = std::make_pair("sub", regVec);
        break;
    case TMUL:
        pairToAdd = std::make_pair("mul", regVec);
        break;
    case TDIV:
        pairToAdd = std::make_pair("div", regVec);
        break;
    default:
        break;
    }
    
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
    context.AddToIntervalsMap(mLhs->GetResultRegister());
    context.AddToIntervalsMap(mRhs->GetResultRegister());
}

void NArrayExpr::CodeGen(CodeContext& context)
{
    //CodeGen the subscript
    mSubscript->CodeGen(context);
    
    //Base address virtual reg
    int baseAddress = context.mStackIndexMatches[mName];
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    
    //Register in which to store the sum of the base address and the subscript
    std::string newReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    
    //Result Register for ArrayExpr
    mResultRegister = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    
    //Movei the base address to the first virtual reg
    std::vector<std::string> regVec = {virtualReg, std::to_string(baseAddress)};
    std::pair<std::string, std::vector<std::string>> pairToAdd;
    pairToAdd = std::make_pair("movi", regVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
    
    //Get the subscript register
    std::string subscriptRegister = mSubscript->GetResultRegister();
    
    //Add the base address and the value computed by the subscript expression
    std::vector<std::string> addVec = {newReg, virtualReg, subscriptRegister};
    
    std::pair<std::string, std::vector<std::string>> pairToSum;
    pairToAdd = std::make_pair("add", addVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(newReg);
    context.AddToIntervalsMap(virtualReg);
    context.AddToIntervalsMap(subscriptRegister);
    
    //Load from the computed address into the mResult register for the ArrayExpr
    std::vector<std::string> loadVec = {mResultRegister, newReg};
    pairToAdd = std::make_pair("load", loadVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(mResultRegister);
    context.AddToIntervalsMap(newReg);
}

void NAssignVarStmt::CodeGen(CodeContext& context)
{
    mRhs->CodeGen(context);
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    int stackIndex = context.mStackIndexMatches[mName];
    
    std::vector<std::string> regVec = {std::to_string(stackIndex), mRhs->GetResultRegister()};
    std::pair<std::string, std::vector<std::string>> pairToAdd;
    pairToAdd = std::make_pair("storei", regVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(mRhs->GetResultRegister());
}

void NAssignArrayStmt::CodeGen(CodeContext& context)
{
    //CodeGen the rhs expression
    mRhs->CodeGen(context);
    //CodeGen the subscript
    mSubscript->CodeGen(context);
    
    //Base address virtual reg
    int baseAddress = context.mStackIndexMatches[mName];
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    
    //Register in which to store the sum of the base address and the subscript
    std::string newReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    
    //Movei the base address to the first virtual reg
    std::vector<std::string> regVec = {virtualReg, std::to_string(baseAddress)};
    std::pair<std::string, std::vector<std::string>> pairToAdd;
    pairToAdd = std::make_pair("movi", regVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
    
    //Get the subscript register
    std::string subscriptRegister = mSubscript->GetResultRegister();
    
    //Add the base address and the value computed by the subscript expression
    std::vector<std::string> addVec = {newReg, virtualReg, subscriptRegister};
    std::pair<std::string, std::vector<std::string>> pairToSum;
    pairToAdd = std::make_pair("add", addVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(newReg);
    context.AddToIntervalsMap(virtualReg);
    context.AddToIntervalsMap(subscriptRegister);
    
    std::vector<std::string> storeVec = {newReg, mRhs->GetResultRegister()};
    pairToAdd = std::make_pair("store", storeVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(newReg);
    context.AddToIntervalsMap(mRhs->GetResultRegister());
}

void NIncStmt::CodeGen(CodeContext& context)
{
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    int stackIndex = context.mStackIndexMatches[mName];
    std::vector<std::string> regVec = {virtualReg, std::to_string(stackIndex)};
    std::pair<std::string, std::vector<std::string>> pairToAdd = std::make_pair("loadi", regVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
    std::vector<std::string> incVec = {virtualReg};
    pairToAdd = std::make_pair("inc", incVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
    std::vector<std::string> storeVec = {std::to_string(stackIndex), virtualReg};
    pairToAdd = std::make_pair("storei", storeVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
}

void NDecStmt::CodeGen(CodeContext& context)
{
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    int stackIndex = context.mStackIndexMatches[mName];
    std::vector<std::string> regVec = {virtualReg, std::to_string(stackIndex)};
    std::pair<std::string, std::vector<std::string>> pairToAdd = std::make_pair("loadi", regVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
    std::vector<std::string> incVec = {virtualReg};
    pairToAdd = std::make_pair("dec", incVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
    std::vector<std::string> storeVec = {std::to_string(stackIndex), virtualReg};
    pairToAdd = std::make_pair("storei", storeVec);
    context.mOps.emplace_back(pairToAdd);
    context.AddToIntervalsMap(virtualReg);
}

void NComparison::CodeGen(CodeContext& context)
{
    mLhs->CodeGen(context);
    mRhs->CodeGen(context);
    std::string lhsRegister = mLhs->GetResultRegister();
    std::string rhsRegister = mRhs->GetResultRegister();
    std::vector<std::string> regVec = {lhsRegister, rhsRegister};
    std::pair<std::string, std::vector<std::string>> pairToAdd;
    switch (mType) {
        case TLESS:
            pairToAdd = std::make_pair("cmplt", regVec);
            context.mOps.emplace_back(pairToAdd);
            break;
        case TISEQUAL:
            pairToAdd = std::make_pair("cmpeq", regVec);
            context.mOps.emplace_back(pairToAdd);
            break;
        default:
            break;
    }
    context.AddToIntervalsMap(lhsRegister);
    context.AddToIntervalsMap(rhsRegister);
}

void NIfStmt::CodeGen(CodeContext& context)
{
    mComp->CodeGen(context);
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    std::vector<std::string> regVec = {virtualReg};
    std::pair<std::string, std::vector<std::string>> movePair = std::make_pair("movi", regVec);
    std::pair<std::string, std::vector<std::string>> jntPair = std::make_pair("jnt", regVec);
    context.mOps.emplace_back(movePair);
    context.AddToIntervalsMap(virtualReg);
    int jumpIfNotTrueIndex = context.mOps.size()-1;
   
    context.mOps.emplace_back(jntPair);
    context.AddToIntervalsMap(virtualReg);
    if(mElseBlock == nullptr){
        mIfBlock->CodeGen(context);
        std::vector<std::string> updatedRegVec = {virtualReg, std::to_string(context.mOps.size())};
        std::pair<std::string, std::vector<std::string>> updatedMovePair = std::make_pair("movi", updatedRegVec);
        context.mOps[jumpIfNotTrueIndex] = updatedMovePair;
    }
    else{
        mIfBlock->CodeGen(context);
        std::string virtualReg2 = "%" + std::to_string(context.mLastRegIndexUsed);
        ++context.mLastRegIndexUsed;
        std::vector<std::string> regVec2 = {virtualReg2};
        std::pair<std::string, std::vector<std::string>> movePair2 = std::make_pair("movi", regVec2);
        std::pair<std::string, std::vector<std::string>> jmpPair = std::make_pair("jmp", regVec2);
        context.mOps.emplace_back(movePair2);
        context.AddToIntervalsMap(virtualReg2);
        int jmpIndex = context.mOps.size()-1;
        context.mOps.emplace_back(jmpPair);
        context.AddToIntervalsMap(virtualReg2);
        std::vector<std::string> updatedRegVec = {virtualReg, std::to_string(context.mOps.size())};
        std::pair<std::string, std::vector<std::string>> updatedMovePair = std::make_pair("movi", updatedRegVec);
        context.mOps[jumpIfNotTrueIndex] = updatedMovePair;
        mElseBlock->CodeGen(context);
        std::vector<std::string> updatedRegVec2 = {virtualReg2, std::to_string(context.mOps.size())};
        std::pair<std::string, std::vector<std::string>> updatedMovePair2 = std::make_pair("movi", updatedRegVec2);
        context.mOps[jmpIndex] = updatedMovePair2;
    }
}

void NWhileStmt::CodeGen(CodeContext& context)
{
    int whileIndex = context.mOps.size();
    mComp->CodeGen(context);
    
    std::string virtualReg = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    std::vector<std::string> regVec = {virtualReg};
    std::pair<std::string, std::vector<std::string>> movePair = std::make_pair("movi", regVec);
    std::pair<std::string, std::vector<std::string>> jntPair = std::make_pair("jnt", regVec);
    context.mOps.emplace_back(movePair);
    context.AddToIntervalsMap(virtualReg);
    int jumpIfNotTrueIndex = context.mOps.size()-1;
    context.mOps.emplace_back(jntPair);
    context.AddToIntervalsMap(virtualReg);
    mBlock->CodeGen(context);
    std::string virtualReg2 = "%" + std::to_string(context.mLastRegIndexUsed);
    ++context.mLastRegIndexUsed;
    std::vector<std::string> regVec2 = {virtualReg2};
    std::pair<std::string, std::vector<std::string>> movePair2 = std::make_pair("movi", regVec2);
    std::pair<std::string, std::vector<std::string>> jmpPair = std::make_pair("jmp", regVec2);
    context.mOps.emplace_back(movePair2);
    context.AddToIntervalsMap(virtualReg2);
    int jmpIndex = context.mOps.size()-1;
    context.mOps.emplace_back(jmpPair);
    context.AddToIntervalsMap(virtualReg2);
    std::vector<std::string> updatedRegVec = {virtualReg, std::to_string(context.mOps.size())};
    std::pair<std::string, std::vector<std::string>> updatedMovePair = std::make_pair("movi", updatedRegVec);
    context.mOps[jumpIfNotTrueIndex] = updatedMovePair;
    std::vector<std::string> updatedRegVec2 = {virtualReg2, std::to_string(whileIndex)};
    std::pair<std::string, std::vector<std::string>> updatedMovePair2 = std::make_pair("movi", updatedRegVec2);
    context.mOps[jmpIndex] = updatedMovePair2;
}

void NPenUpStmt::CodeGen(CodeContext& context)
{
    std::vector<std::string> regVec = {""};
    std::pair<std::string, std::vector<std::string>> pair = std::make_pair("penup", regVec);
    context.mOps.emplace_back(pair);
}

void NPenDownStmt::CodeGen(CodeContext& context)
{
    std::vector<std::string> regVec = {""};
    std::pair<std::string, std::vector<std::string>> pair = std::make_pair("pendown", regVec);
    context.mOps.emplace_back(pair);
}

void NSetPosStmt::CodeGen(CodeContext& context)
{
    mXExpr->CodeGen(context);
    mYExpr->CodeGen(context);
    std::string xReg = mXExpr->GetResultRegister();
    std::string yReg = mYExpr->GetResultRegister();
    std::vector<std::string> xRegVec = {"tx", xReg};
    std::vector<std::string> yRegVec = {"ty", yReg};
    std::pair<std::string, std::vector<std::string>> xPair = std::make_pair("mov", xRegVec);
    std::pair<std::string, std::vector<std::string>> yPair = std::make_pair("mov", yRegVec);
    context.mOps.emplace_back(xPair);
    context.AddToIntervalsMap(xReg);
    context.mOps.emplace_back(yPair);
    context.AddToIntervalsMap(yReg);
}

void NSetColorStmt::CodeGen(CodeContext& context)
{
    mColor->CodeGen(context);
    std::string colorReg = mColor->GetResultRegister();
    std::vector<std::string> colorRegVec = {"tc", colorReg};
    std::pair<std::string, std::vector<std::string>> colorPair = std::make_pair("mov", colorRegVec);
    context.mOps.emplace_back(colorPair);
    context.AddToIntervalsMap(colorReg);
}

void NFwdStmt::CodeGen(CodeContext& context)
{
    mParam->CodeGen(context);
    std::string reg = mParam->GetResultRegister();
    std::vector<std::string> regVec = {reg};
    std::pair<std::string, std::vector<std::string>> pair = std::make_pair("fwd", regVec);
    context.mOps.emplace_back(pair);
    context.AddToIntervalsMap(reg);
    
}

void NBackStmt::CodeGen(CodeContext& context)
{
    mParam->CodeGen(context);
    std::string reg = mParam->GetResultRegister();
    std::vector<std::string> regVec = {reg};
    std::pair<std::string, std::vector<std::string>> pair = std::make_pair("back", regVec);
    context.mOps.emplace_back(pair);
    context.AddToIntervalsMap(reg);
}

void NRotStmt::CodeGen(CodeContext& context)
{
    mParam->CodeGen(context);
    std::string reg = mParam->GetResultRegister();
    std::vector<std::string> regVec = {"tr", "tr", reg};
    std::pair<std::string, std::vector<std::string>> pair = std::make_pair("add", regVec);
    context.mOps.emplace_back(pair);
    context.AddToIntervalsMap(reg);
}
