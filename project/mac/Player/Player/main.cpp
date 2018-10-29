//
//  main.cpp
//  Player
//
//  Created by 陈龙 on 2018/10/29.
//  Copyright © 2018 陈龙. All rights reserved.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "src/LLWindow.h"
#include "src/DecodeVideo.h"

using namespace std;

#define SRC_FILE "../wangzhe.mp4"

int main(int argc, char* argv[])
{
    
    FormatCtx fmt_ctx(SRC_FILE);
    fmt_ctx.init_info();
    
    LLWindow window(fmt_ctx.width(), fmt_ctx.height());
    
    
    DecodeVideo dv(fmt_ctx);
    thread th([&window, &dv](){
        this_thread::sleep_for(chrono::milliseconds(2000));
        dv.decode_video(&window);
    });
    th.detach();
    
    window.init_window();
    return 0;
}

