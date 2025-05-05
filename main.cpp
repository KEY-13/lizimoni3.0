#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <conio.h>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include "ParticleSystem.h"
#include "GUI.h"

class Game {
    ParticleSystem ps;
    GUI gui;
    int brushSize = 5;
    bool isRunning = true;
    DWORD lastFrame = 0;

public:
    Game() {
        gui.setClearCallback([this]() { ps.clearAll(); });
        gui.setSelectedMat(SAND);  // 默认材质
    }

    void run() {
        initgraph(1280, 720, SHOWCONSOLE);
        SetWindowText(GetHWnd(), "粒子沙盒 v3.0 (优化版)");
        BeginBatchDraw();

        while (isRunning) {
            handleInput();
            update();
            render();
        }

        EndBatchDraw();
        closegraph();
    }

private:
    void handleInput() {
        static DWORD lastAdd = 0;

        // 键盘输入处理
        if (_kbhit()) {
            int key = _getch();
            if (key == 27) isRunning = false;  // ESC退出
            if (key == '=') brushSize = std::min(20, brushSize + 1);
            if (key == '-') brushSize = std::max(1, brushSize - 1);
            if (key >= '1' && key <= '4') {    // 数字键切换材质
                gui.setSelectedMat(static_cast<Material>(key - '1'));
            }
        }

        // 鼠标输入处理
        while (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            gui.handleInput(msg);

            if (msg.mkLButton && !gui.isMouseOverUI(msg.x, msg.y)) {
                // 生成节流：每30ms生成一批
                if (GetTickCount() - lastAdd > 30) {
                    ps.addParticles(msg.x, msg.y, brushSize, gui.getSelectedMat());
                    lastAdd = GetTickCount();
                }
            }
        }
    }

    void update() {
        ps.update();
    }

    void render() {
        // 帧率控制：保持60FPS
        DWORD current = GetTickCount();
        if (current - lastFrame < 16) return;

        cleardevice();

        // 半透明虚线网格
        setlinestyle(PS_DOT, 1);  // 点状虚线
        setlinecolor(0x40AAAAAA); // 半透明度0x40（25%）

        // 稀疏网格（间隔40像素）
        for (int y = 0; y < 720; y += 40)
            line(0, y, 1280, y);
        for (int x = 0; x < 1280; x += 40)
            line(x, 0, x, 720);

        // 恢复默认样式
        setlinestyle(PS_SOLID, 1);
        setlinecolor(BLACK);

        // 绘制背景网格
        /*
        setlinecolor(RGB(240, 240, 240));
        for (int y = 0; y < 720; y += 20) line(0, y, 1280, y);
        for (int x = 0; x < 1280; x += 20) line(x, 0, x, 720);
        */

        // 绘制粒子系统
        ps.draw();

        // 绘制UI界面
        gui.draw(brushSize);

        // 性能监控
        settextcolor(BLACK);
        outtextxy(10, 280, ("粒子数量: " + std::to_string(ps.getCount())).c_str());
        outtextxy(10, 300, ("FPS: " + std::to_string(1000 / (current - lastFrame))).c_str());

        FlushBatchDraw();
        lastFrame = current;
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
