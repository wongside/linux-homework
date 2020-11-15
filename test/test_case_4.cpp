#include <memory>
#include "../data_field/field.h"
#include "ctime"
#include "../file_manager/file_manager.h"

using namespace wang::side;

void Storage(wang::side::UserAttrVector *v) {
    int total_bytes = v->GetBufferSize();
    char *buff = new char[total_bytes];
    v->ToCharBuffer(buff);
    auto *p = new WriterMethodParam;
    p->Buff = buff;
    p->Size = total_bytes;
    p->Callback = [](int row, const char *buf, int size) {
        std::cout << "write row:" << row << ",size:" << size << " success" << std::endl;
        delete[]buf;
    };
    wang::side::FileManager::GetInstance()->MultiWrite(p);
}

int main() {
    srand((unsigned int) time(nullptr));
    int total_row = 0;
    //construct 100 attribute type of one row
    std::vector<FieldType> column_type;
    column_type.reserve(100);
    for (int i = 0; i < 100; i++) {
        column_type.push_back(FieldType::Integer64);
    }

    std::unique_ptr<wang::side::UserAttrVector> userInfo[total_row];
    for (int i = 0; i < total_row; i++) {
        userInfo[i].reset(new UserAttrVector(column_type));
    }
    FieldProduceFactory factory;

    wang::side::FileManager::InitInstance(1000, sizeof(int) * 100);
    for (int i = 0; i < total_row; i++) {
        auto &v = userInfo[i];
        for (int j = 1; j <= 100; j++) {
            auto *intObject = factory.CreateField(rand() % 100);
            v->PushBack(intObject);
        }
        Storage(v.get());
    }
    int size = 400;
    char buf[size];
    wang::side::FileManager::GetInstance()->Read(1994, buf, size);
    UserAttrVector desVec(column_type);
    desVec.FromCharBuffer(buf);
    desVec.Print();
    return 0;
}
