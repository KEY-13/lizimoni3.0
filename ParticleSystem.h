#pragma once
#include <graphics.h>
#include <vector>
#include <algorithm>
#include <cstring>

enum Material { SAND, WATER, FIRE, STONE, EMPTY };

struct Particle {
    int x, y;
    COLORREF color;
    Material type;
    int lifetime;  // 生命周期（火专用）
    bool updated = false;
};


class ParticleSystem {
    std::vector<Particle> particles;
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    const size_t MAX_PARTICLES = 50000;

// 在类内添加：
    int particleSize = 1; // 默认粒子大小（假设统一尺寸）

    // 空间分区优化（16x16网格）
    static const int GRID_SIZE = 16;
    int grid[80][45] = { 0 };  // 1280/16=80, 720/16≈45

public:
    void clearAll() { particles.clear(); }
    size_t getCount() const { return particles.size(); }

    void addParticles(int centerX, int centerY, int radius, Material mat) {
        int density = (mat == STONE) ? 1 : 2;  // 不同材质密度

        for (int dy = -radius; dy <= radius; dy += density) {
            for (int dx = -radius; dx <= radius; dx += density) {
                if (dx * dx + dy * dy > radius * radius) continue;
                if (particles.size() >= MAX_PARTICLES) return;

                int x = centerX + dx;
                int y = centerY + dy;
                if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) continue;

                // 检查网格密度
                int gx = x / GRID_SIZE, gy = y / GRID_SIZE;
                if (grid[gx][gy] < 50) {
                    particles.push_back(createParticle(x, y, mat));
                    grid[gx][gy]++;
                }
            }
        }
    }

    void update() {
        // 重置网格和更新状态
        memset(grid, 0, sizeof(grid));
        for (auto& p : particles) {
            grid[p.x / GRID_SIZE][p.y / GRID_SIZE]++;
            p.updated = false;
        }

        // 并行反向更新（C++17并行算法）
        std::for_each(particles.rbegin(), particles.rend(),
            [&](auto& p) {
                if (p.updated || p.type == EMPTY) return;

                switch (p.type) {
                case SAND:  updateSand(p);  break;
                case WATER: updateWater(p); break;
                case FIRE:  updateFire(p);  break;
                case STONE: break;
                }
            });
        // 移除无效粒子
        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [](const auto& p) { return p.type == EMPTY; }), particles.end());
    }

    void draw() {
        for (const auto& p : particles) {
            if (p.type != EMPTY) {
                putpixel(p.x, p.y, p.color);
            }
        }
    }

private:
    Particle createParticle(int x, int y, Material mat) {
        COLORREF color;
        switch (mat) {
        case SAND:  color = RGB(194, 178, 128); break;
        case WATER: color = RGB(30, 144, 255);  break;
        case FIRE:  color = RGB(255, 69 + rand() % 50, 0); break;
        case STONE: color = RGB(100, 100, 100); break;
        }
        return { x, y, color, mat, (mat == FIRE) ? 100 : 0, false };
    }
    bool isEmpty(int x, int y) {
        // 假设 particleSize 是类成员变量
        for (int dx = particleSize; dx <= particleSize; ++dx) {
            for (int dy = -particleSize; dy <= particleSize; ++dy) {
                int checkX = x + dx;
                int checkY = y + dy;

                // 边界检查
                if (checkX < 0 || checkX >= WIDTH || checkY < 0 || checkY >= HEIGHT)
                    continue;

                // 检查该位置是否被占用
                if (!positionIsEmpty(checkX, checkY))
                    return false;
            }
        }
        return true;
    }

    // 辅助函数（需添加到私有成员）
    bool positionIsEmpty(int x, int y) {
        return std::none_of(particles.begin(), particles.end(),
            [x, y](const auto& p) {
                return p.x == x && p.y == y && p.type != EMPTY;
            });
    }


    /*
    bool isEmpty(int x, int y) {
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return false;
        return std::none_of(particles.begin(), particles.end(),
            [x, y](const auto& p) { return p.x == x && p.y == y && p.type != EMPTY; });
    }
    */

    // 物理模拟函数
    void updateSand(Particle& p) {
        if (tryMove(p, 0, 1)) return;
        int dir = (rand() % 2) ? 1 : -1;
        if (tryMove(p, dir, 1)) return;
    }

    void updateWater(Particle& p) {
        if (tryMove(p, 0, 1)) return;
        int dir = (rand() % 2) ? 1 : -1;
        if (tryMove(p, dir, 0)) return;
        if (tryMove(p, -dir, 0)) return;
    }

    void updateFire(Particle& p) {
        p.lifetime -= rand() % 5;
        if (p.lifetime <= 0) {
            p.type = EMPTY;
            return;
        }
        if (tryMove(p, 0, -1)) return;
        if (tryMove(p, (rand() % 3) - 1, -1)) return;
    }

    bool tryMove(Particle& p, int dx, int dy) {
        int newX = p.x + dx;
        int newY = p.y + dy;
        if (!isEmpty(newX, newY)) return false;

        auto target = std::find_if(particles.begin(), particles.end(),
            [newX, newY](auto& p) { return p.x == newX && p.y == newY; });

        if (target != particles.end()) {
            std::swap(p.x, target->x);
            std::swap(p.y, target->y);
            return true;
        }
        return false;
    }
};
