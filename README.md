# Big Fish Eat Small Fish
*Demo game: https://drive.google.com/drive/folders/1NaWhtO3xErOVb4dj8DTppbDdE1qUo8rx?usp=drive_link
## Giới thiệu
Big Fish Eat Small Fish là một game arcade 2D viết bằng C++ với SDL2. Bạn sẽ vào vai một chú cá nhỏ, bơi khắp đại dương, ăn những con cá yếu hơn để lớn lên, đồng thời né tránh và cạnh tranh với những con cá khổng lồ hơn. ![Uploading image.png…]()

# 0. Cách tải và chạy game
## a. Chạy bản nhị phân (không bao gồm code)
- Tải folder trên GitHub (khoảng 20 MB).  
- Chạy **BigFishEatSmallFish.exe** (Windows) hoặc `./BigFishEatSmallFish` (Linux/macOS).
## b. Chạy bản có source code
1. Clone toàn bộ repo:
    ```bash
    git clone https://github.com/hvu34/DED.git
    cd DED
    ```
2. Cài đặt SDL2 + SDL2_image + SDL2_ttf + SDL2_mixer.  
3. Build với CMake:
    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build . --config Release
    ```
   hoặc mở file `.sln` bằng Visual Studio, chọn Release và **Build**.  
4. Chạy binary ở thư mục `Release/`.

# 1. Bắt đầu game

1. **Mở game** → xuất hiện menu chính.  
2. Click **PLAY** hoặc nhấn Enter để vào game.  
3. Game sẽ hiển thị background đầu tiên và chú cá Player ở vị trí mặc định.
4. ![image](https://github.com/user-attachments/assets/71bb8ce8-e8a9-4481-8599-ef1b7a2bdc9c)


# 2. Điều khiển

  Phím/Nút     | Hành động                       
 
  ← ↑ → ↓      | Di chuyển trái, lên, phải, xuống 
  ESC          | Thoát game                      
  Chuột        | Chọn các nút trong menu        
# 3. Các thành phần trong game
- Player (cá của bạn)
  - Bắt đầu kích thước: 64×64.  
  - Tăng kích thước khi ăn cá nhỏ hơn.  
  - Điểm = (width×height)/100.
  - ![image](https://github.com/user-attachments/assets/661be5a2-490e-4a60-8380-f1eb0630231c)

- Cá NPC thông thường
  - Spawn từ hai bên rìa màn hình, không spawn gần Player.  
  - 80 % kích thước = 80 % của Player.  
  - 20 % kích thước > Player (tăng thêm 25–50 %).  
  - Di chuyển vào trong màn hình với vận tốc ngẫu nhiên.
  - ![image](https://github.com/user-attachments/assets/dfea416f-4529-4b69-be6b-170093c72390)

- Cá đặc biệt
  - Xuất hiện mỗi khi Player đạt thêm 100 điểm.  
  - Kích thước sao cho điểm = PlayerPts + 100.  
  - Texture mặc định: `Images/shark real.png`.
  - ![image](https://github.com/user-attachments/assets/7853ddbe-8ddf-4987-bc8c-f04bf58d1281)

- Background  
  - Khi lên mỗi level, background đổi tuần tự giữa back.jpg → back1.jpg → back2.jpg → back3.jpg → back4.jpg → back.jpg → …
  - ![image](https://github.com/user-attachments/assets/d6cc1af8-a4a5-452e-8881-fe00174480ca)


# 4. Cách chơi

- Ăn cá nhỏ: để tăng kích thước và tích lũy điểm.  
- Tránh những con cá lớn: hơn mình, chạm phải sẽ thua.  
- Mỗi 5 con cá ăn được: sẽ lên 1 level, tăng tốc spawn và đổi background.  
- Cá đặc biệt: sẽ xuất hiện để thách thức: bạn phải có điểm cao hơn nó để ăn.

---

# 5. Chiến thắng & thất bại

- Thắng: Chạm đủ số điểm mục tiêu (ví dụ 500 điểm) hoặc tiêu diệt cá đặc biệt cuối cùng.  
- Thua: Bị cá lớn ăn hoặc di chuyển ra khỏi màn hình -> chọn play again hoặc quit.
![image](https://github.com/user-attachments/assets/76cc3ec5-9607-4882-afdc-a5b08892d727)

---

# 6. Cấu trúc thư mục
![image](https://github.com/user-attachments/assets/a190e036-1cf9-4278-aebf-87cb333f68f6)

