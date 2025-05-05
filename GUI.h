#pragma once
#include <graphics.h>
#include <string>
#include <functional>

class ParticleSystem;  // 前向声明

class GUI {
    Material selectedMat = SAND;
    std::function<void()> clearCallback;

public:
    void setClearCallback(std::function<void()> cb) { clearCallback = cb; }
    void setSelectedMat(Material mat) { selectedMat = mat; }
    Material getSelectedMat() const { return selectedMat; }

    void handleInput(const MOUSEMSG& msg) {
        if (msg.uMsg == WM_LBUTTONDOWN) {
            // 材质选择按钮区域 (修正参数范围)
            if (isInRect(msg.x, msg.y, 10, 10, 160, 170)) {
                // 修正按钮索引计算（每个按钮高度40像素）
                int buttonIndex = (msg.y - 10) / 40;
                switch (buttonIndex) {
                case 0: selectedMat = SAND; break;
                case 1: selectedMat = WATER; break;
                case 2: selectedMat = FIRE; break;
                case 3: selectedMat = STONE; break;
                default: break; // 处理超出范围的情况
                }
            }

            // 清空按钮（修正坐标范围）
            if (isInRect(msg.x, msg.y, 10, 180, 80, 220) && clearCallback) {
                clearCallback();
            }
        }
    }

    void draw(int brushSize) const {
        setlinestyle(PS_SOLID, 2);
        settextstyle(14, 0, _T("Consolas"));

        // 绘制材质面板（修正按钮位置）
        fillrectangle(10, 10, 160, 170);
        drawMaterialButton(10, SAND);    // 参数修正
        drawMaterialButton(50, WATER);   // 参数修正
        drawMaterialButton(90, FIRE);    // 参数修正
        drawMaterialButton(130, STONE);  // 参数修正

        // 信息面板（添加类型转换）
        settextcolor(BLACK);
        outtextxy(10, 200, ("当前材质: " + getMatName()).c_str());
        outtextxy(10, 220, ("刷子大小: " + std::to_string(brushSize)).c_str());
    }

    bool isMouseOverUI(int x, int y) const {
        return isInRect(x, y, 10, 10, 160, 220);
    }

private:
    // 修正函数参数（移除冗余参数）
    void drawMaterialButton(int yPos, Material mat) const {
        const char* names[] = { "沙子 (1)", "水 (2)", "火 (3)", "石头 (4)" };
        COLORREF colors[] = {
            RGB(194,178,128), RGB(30,144,255),
            RGB(255,100,0), RGB(100,100,100)
        };

        // 背景（添加类型转换）
        setfillcolor((mat == selectedMat) ? colors[static_cast<int>(mat)] : RGB(240, 240, 240));
        solidrectangle(10, yPos, 160, yPos + 40);

        // 材质预览
        setfillcolor(colors[static_cast<int>(mat)]);  // 显式类型转换
        solidcircle(30, yPos + 20, 12);

        // 文字说明
        settextcolor((mat == selectedMat) ? WHITE : BLACK);
        outtextxy(50, yPos + 12, names[static_cast<int>(mat)]);  // 类型转换
    }

    std::string getMatName() const {
        switch (selectedMat) {
        case SAND:  return "沙子";
        case WATER: return "水";
        case FIRE:  return "火";
        case STONE: return "石头";
        default:     return "未知";  // 处理默认情况
        }
    }

    bool isInRect(int x, int y, int x1, int y1, int x2, int y2) const {
        return x >= x1 && x <= x2 && y >= y1 && y <= y2;
    }
};
