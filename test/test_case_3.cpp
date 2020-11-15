#include <memory>
#include <random>
#include "../data_field/field.h"
#include "ctime"
#include "../file_manager/file_manager.h"

using namespace wang::side;

void Storage(wang::side::UserAttrVector *v) {
    int totalBytes = v->GetBufferSize();
    char *buf = new char[totalBytes];
    v->ToCharBuffer(buf);
    auto *p = new WriterMethodParam;
    p->Buff = buf;
    p->Size = totalBytes;
    p->Callback = [](int row, const char *buf, int size) {
        std::cout << "The " << row << "th data is written successfully" << std::endl;
        delete[]buf;
    };
    wang::side::FileManager::GetInstance()->MultiWrite(p);
}

int main() {

    srand((unsigned int) time(nullptr));

    int total_row = 1000000;
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
    return 0;
}
