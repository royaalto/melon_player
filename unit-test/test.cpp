#include <stdint.h>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    //#include "libavutil/time.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
    #include "time1.h"
}
#include "metadata.hpp"


// 测试集为 MyTest，测试案例为 Sum
TEST(test, xx)
{

    Mellon::Metadata md;
    QString file = "/home/roy/Videos/5.mp4";
    bool test = Mellon::getMetadata(file,md);
    EXPECT_EQ(true, test);
}
int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}