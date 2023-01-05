#include "SrcMain.h"
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include "Node.h"
#include <fstream>

extern int proccparse(); // NOLINT
struct yy_buffer_state; // NOLINT
extern void procc_flush_buffer(yy_buffer_state* b); // NOLINT
extern int procclex_destroy(); // NOLINT
extern FILE* proccin; // NOLINT

extern int gLineNumber;
extern NProgram* gProgram;
bool gSyntaxError = false;

// CHANGE ANYTHING ABOVE THIS LINE AT YOUR OWN RISK!!!!

int ProcessCommandArgs(int argc, const char* argv[])
{
    gLineNumber = 1;
    gSyntaxError = false;
    if (argc < 2)
    {
        std::cout << "You must pass the input file as a command line parameter." << std::endl;
        return 1;
    }
    
    // Read input from the first command line parameter
    proccin = fopen(argv[1], "r");
    if (proccin == nullptr)
    {
        std::cout << "File not found: " << argv[1] << std::endl;
        return 1;
    }
    procc_flush_buffer(nullptr);
    
    // Start the parse
    proccparse();
    
    if (gProgram != nullptr && argc == 3)
    {
        // TODO: Add any needed code for Parts 2, 3, and 4!!!
        std::ofstream outputFile("ast.txt");
        if(outputFile.is_open()){
            gProgram->OutputAST(outputFile, 0);
        }
        outputFile.close();
        std::string argvIndex2 = argv[2];
        if(argvIndex2.find("emit") != std::string::npos){
            CodeContext c;
            gProgram->CodeGen(c);
            std::ofstream emitFile("emit.txt");
            std::string opName;
            std::vector<std::string> regNames;
            if(emitFile.is_open()){
                for(auto elem : c.mOps){
                    opName = elem.first;
                    regNames = elem.second;
                    if(!(regNames[0].empty()) && opName != "exit"){
                        emitFile << opName << " ";
                    }
                    else{
                        emitFile << opName << std::endl;
                    }
                    if(!regNames.empty()){
                        if(!(regNames[0].empty()) && opName != "exit"){
                            for(int i = 0; i < regNames.size()-1; i++){
                                emitFile << regNames[i] << ",";
                            }
                            emitFile << regNames[regNames.size()-1] << std::endl;
                        }
                    }
                }
            }
            emitFile.close();
        }
        else if(argvIndex2.find("reg") != std::string::npos){
            CodeContext c;
            gProgram->CodeGen(c);
            c.AllocateRegisters();
            std::ofstream regFile("reg.txt");
            if(regFile.is_open()){
                regFile << "INTERVALS:" << std::endl;
                for(auto item: c.mIntervals){
                    regFile << "%" << item.first << ":";
                    regFile << item.second[0] << ",";
                    regFile << item.second.back() << std::endl;
                }
                regFile << "ALLOCATION:" << std::endl;
                for(auto item: c.mRegisterPairs){
                    regFile << "%" << item.first << ":";
                    regFile << item.second << std::endl;
                }
            }
            regFile.close();
            std::ofstream emitFile("emit.txt");
            std::string opName;
            std::vector<std::string> regNames;
            if(emitFile.is_open()){
                for(auto elem : c.mOps){
                    opName = elem.first;
                    regNames = elem.second;
                    if(!(regNames[0].empty()) && opName != "exit"){
                        emitFile << opName << " ";
                    }
                    else{
                        emitFile << opName << std::endl;
                    }
                    if(!regNames.empty()){
                        if(!(regNames[0].empty()) && opName != "exit"){
                            for(int i = 0; i < regNames.size()-1; i++){
                                if(regNames[i][0]=='%'){
                                    int regNameAsInt = std::stoi(regNames[i].substr(1));
                                    std::string realRegister = c.mRegisterPairs[regNameAsInt];
                                    emitFile << realRegister << ",";
                                }
                                else{
                                    emitFile << regNames[i] << ",";
                                }
                            }
                            if(regNames[(regNames.size()-1)][0]=='%'){
                                int regNameAsInt = std::stoi(regNames[(regNames.size()-1)].substr(1));
                                std::string realRegister = c.mRegisterPairs[regNameAsInt];
                                emitFile << realRegister << std::endl;
                            }
                            else{
                                emitFile << regNames[regNames.size()-1] << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // (Just a useful separator for debug cout statements in grammar)
        std::cout << "**********************************************\n";
    }
    
    // Close the file stream
    fclose(proccin);
    // Destroy lexer so it reinitializes
    procclex_destroy();
    // Return 1 if syntax error, 0 otherwise
    return static_cast<int>(gSyntaxError);
}

void proccerror(const char* s) // NOLINT
{
    std::cout << s << " on line " << gLineNumber << std::endl;
    gSyntaxError = true;
}
