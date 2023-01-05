#pragma once
#include <string>
#include <utility>
#include <functional>

template <typename ValueType, 
typename HashFuncType = std::hash<std::string>>
class HashTable
{
public:
    // Constructs the hash table with the given capacity (size of underlying array)
    HashTable(size_t capacity)
    :mTableData(new std::pair<std::string, ValueType>[capacity])
    ,mCapacity(capacity)
    ,mSize(0)
    { }
    
    // TODO: Implement "Rule of five" functions
    //Destructor
    ~HashTable(){
        delete [] mTableData;
        mTableData = nullptr;
        mSize = 0;
        mCapacity = 0;
    }
    
    //Copy Constructor
    HashTable(const HashTable& other)
    : mTableData(new std::pair<std::string, ValueType>[other.mCapacity])
    , mCapacity(other.mCapacity)
    , mSize(other.mSize) {
        for(size_t i = 0; i < other.mCapacity; i++){
            mTableData[i] = other.mTableData[i];
        }
    }
    //Assignment Operator
    HashTable& operator=(const HashTable& other){
        if(&other != this){
            delete [] mTableData;
            mSize = other.mSize;
            mCapacity = other.mCapacity;
            mTableData = new std::pair<std::string, ValueType>[mCapacity];
            for(size_t i = 0; i < other.mCapacity; i++){
                mTableData[i] = other.mTableData[i];
            }
        }
        return *this;
    }
    
    //Move Constructor
    HashTable(HashTable&& other)
    : mTableData(std::move(other.mTableData))
    , mCapacity(std::move(other.mCapacity))
    , mSize(std::move(other.mSize))
    {
        other.mTableData = nullptr;
        other.mSize = 0;
        other.mCapacity = 0;
    }
    
    //Move Assignment Operator
    HashTable& operator=(HashTable&& other){
        if(&other != this){
            mTableData = std::move(other.mTableData);
            mCapacity = std::move(other.mCapacity);
            mSize = std::move(other.mSize);
            other.mTableData = nullptr;
            other.mSize = 0;
            other.mCapacity = 0;
        }
        return *this;
    }
    
    // Returns overall capacity of hash table (size of underlying array)
    size_t Capacity() const
    {
        return mCapacity;
    }
    
    // Returns the number of elements currently inserted in the hash table
    size_t Size() const
    {
        return mSize;
    }
    
    size_t GetHash(const std::string& key) const{
        size_t hash = mHashFunc(key);
        size_t index = hash % mCapacity;
        return index;
    }
    
    // Tries to insert (key, value) into hash table
    // Returns true if insert succeeds, false if not
    bool Insert(const std::string& key, const ValueType& value)
    {
        size_t index = GetHash(key);
        std::pair<std::string, ValueType> temp = std::make_pair(key, value);
        if(mSize < mCapacity){
            while(!mTableData[index].first.empty() && mTableData[index].first != key){
                index++;
                if(index >= mCapacity){
                    index = 0;
                }
            }
            mTableData[index] = temp;
            mSize += 1;
            return true;
        }
        else{
            return false;
        }
    }
    
    // Returns pointer to value for given key, or nullptr
    // if not in hash table
    ValueType* Find(const std::string& key) const
    {
        size_t index = GetHash(key);
        size_t numChecked = 0;
        while(mTableData[index].first != key){
            index++;
            numChecked += 1;
            if(index >= mCapacity){
                index = 0;
            }
            if(mTableData[index].first.empty() || numChecked == mCapacity){
                return nullptr;
            }
        }
        return &mTableData[index].second;
    }
    
    // Executes provided lambda expression on each element inserted into the table
    void ForEach(std::function<void(const std::string& key, ValueType& value)> func)
    {
        for(size_t i = 0; i < mCapacity; i++){
            if(!mTableData[i].first.empty()){
                func(mTableData[i].first, mTableData[i].second);
            }
        }
    }
    
    // Gets a const pointer to underlying data (for testing)
    const std::pair<std::string, ValueType>* Data() const
    {
        return mTableData;
    }
private:
    // NOTE: you should not need any more member data!
    
    // Underlying hash table array
    std::pair<std::string, ValueType>* mTableData;
    // Allocated capacity of hash table array
    size_t mCapacity;
    // Number of elements in HashTable
    size_t mSize;
    // Hash function
    HashFuncType mHashFunc;
};
