#include <SPI.h>
#include "pitches.h"

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

#define POS_X 0
#define NEG_X 1
#define POS_Z 2
#define NEG_Z 3
#define POS_Y 4
#define NEG_Y 5

#define BUTTON_PIN 8
#define YELLOW_LED 5
#define BLUE_LED 7
#define BUZZER_PIN 9

#define TOTAL_EFFECTS 5
#define RAIN 0
#define PLANE_BOING 1
#define SEND_VOXELS 2
#define WOOP_WOOP 3
#define CUBE_JUMP 4

#define RAIN_TIME music_time/8
#define PLANE_BOING_TIME music_time/8
#define SEND_VOXELS_TIME music_time/16
#define WOOP_WOOP_TIME music_time/4
#define CUBE_JUMP_TIME music_time/8

#define CLOCK_TIME 500

uint8_t cube[8][8];
uint8_t currentEffect;

uint16_t timer;

uint64_t randomTimer;

bool loading;
bool music_loading;

void setup() {

        loading = true;
        music_loading = true;
        randomTimer = 0;

        currentEffect = RAIN;

        SPI.begin();
        SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

        pinMode(BUTTON_PIN, INPUT_PULLUP);
        pinMode(YELLOW_LED, OUTPUT);
        pinMode(BLUE_LED, OUTPUT);

        randomSeed(analogRead(0));
        digitalWrite(BLUE_LED, HIGH);

}

void loop() {

        randomTimer++;

        if (digitalRead(BUTTON_PIN) == LOW) {
                clearCube();
                loading = true;
                timer = 0;
                currentEffect++;
                if (currentEffect == TOTAL_EFFECTS) {
                        currentEffect = 0;
                }
                randomSeed(randomTimer);
                randomTimer = 0;

                digitalWrite(YELLOW_LED, HIGH);
                digitalWrite(BLUE_LED, LOW);
                delay(500);
                digitalWrite(YELLOW_LED, LOW);
                digitalWrite(BLUE_LED, HIGH);
        }
        switch (currentEffect) {
        case RAIN: rain(); break;
        case PLANE_BOING: planeBoing(); break;
        case SEND_VOXELS: sendVoxels(); break;
        case WOOP_WOOP: woopWoop(); break;
        case CUBE_JUMP: cubeJump(); break;
        default: rain();
        }
        renderCube();
        playMusic();
}

uint16_t music_timer;
uint16_t music_time;
uint8_t thisNote;
const uint16_t BTIME=1680;
uint8_t part;
bool repeated;

const uint16_t notes1[] = {
        NOTE_D6, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6,
        NOTE_D6, NOTE_G5, NOTE_G5,
        NOTE_E6, NOTE_C6, NOTE_D6,NOTE_C6, NOTE_D6, NOTE_E6, NOTE_FS6,
        NOTE_G6, NOTE_G5, NOTE_G5,
        NOTE_C6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_A5,
        NOTE_B5, NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G5,
        NOTE_FS5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_G5,
        NOTE_B5, NOTE_A5,0,0,
        NOTE_D6, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6,
        NOTE_D6, NOTE_G5, NOTE_G5,
        NOTE_E6, NOTE_C6, NOTE_D6,NOTE_C6, NOTE_D6, NOTE_E6, NOTE_FS6,
        NOTE_G6, NOTE_G5, NOTE_G5,
        NOTE_C6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_A5,
        NOTE_B5, NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G5,
        NOTE_A5, NOTE_B5, NOTE_A5, NOTE_G5, NOTE_FS5,
        NOTE_G5, 0
};
const uint8_t duration1[] = {
        4,8,8,8,8,
        4,4,4,
        4,32,32,16,8,8,8,
        4,4,4,
        32,32,5,8,8,8,8,
        4,8,8,8,8,
        4,8,8,8,8,
        16,2,8,16,
        4,8,8,8,8,
        4,4,4,
        4,32,32,16,8,8,8,
        4,4,4,
        32,32,5,8,8,8,8,
        4,8,8,8,8,
        4,8,8,8,8,
        2,4
};
const uint16_t notes2[] = {
        NOTE_B6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_G6,
        NOTE_A6, NOTE_D6, NOTE_E6, NOTE_FS6, NOTE_D6,
        NOTE_G6, NOTE_E6, NOTE_FS6, NOTE_G6, NOTE_D6,
        NOTE_CS6, NOTE_B5, NOTE_CS6, NOTE_A5,
        NOTE_A5, NOTE_B5, NOTE_CS6, NOTE_D6, NOTE_E6, NOTE_FS6,
        NOTE_G6, NOTE_FS6, NOTE_E6, NOTE_FS6, NOTE_A5, NOTE_CS6,
        NOTE_D6, 0,
        NOTE_D6, NOTE_G5, NOTE_FS5, NOTE_G5,
        NOTE_E6, NOTE_G5, NOTE_FS5, NOTE_G5,
        NOTE_D6, NOTE_C6, NOTE_B5,
        NOTE_A5, NOTE_G5, NOTE_FS5, NOTE_G5, NOTE_A5,
        NOTE_D5, NOTE_E5, NOTE_FS5, NOTE_G5, NOTE_A5, NOTE_B5,
        NOTE_C6, NOTE_B5,NOTE_D6, NOTE_B5, NOTE_A5,
        NOTE_B5, NOTE_D6, NOTE_G5, NOTE_FS5,
        NOTE_G5,0
};
const uint8_t duration2[] = {
        4,8,8,8,8,
        4,8,8,8,8,
        4,8,8,8,8,
        4,8,8,4,
        8,8,8,8,8,8,
        4,4,4,
        4,4,4,
        2,4,
        4,8,8,4,
        4,8,8,4,
        4,4,4,
        8,8,8,8,4,
        8,8,8,8,8,8,
        4,32,32,5,4,
        8,8,4,4,
        2,4
};

void playMusic(){
        if(music_loading) {
                thisNote=-1;
                part=1;
                repeated=false;
                music_timer=0;
                music_time=BTIME;
                music_loading=false;
        }
        music_timer++;
        if(music_timer>music_time) {
                noTone(BUZZER_PIN);
        }
        if(music_timer>music_time*1.2) {
                thisNote++;
                if(part==1&&thisNote==sizeof(notes1)/sizeof(uint16_t)) {
                        if(repeated) {
                                part=2;
                                repeated=false;
                                thisNote=0;
                                loading = true;
                                timer = 0;
                                randomSeed(randomTimer);
                                randomTimer = 0;
                                currentEffect++;
                        }
                        else{
                                repeated=true;
                                thisNote=0;
                                loading = true;
                                timer = 0;
                                randomSeed(randomTimer);
                                randomTimer = 0;
                                currentEffect++;
                        }
                }
                if(part==2&&thisNote==sizeof(notes2)/sizeof(uint16_t)) {
                        if(repeated) {
                                part=1;
                                repeated=false;
                                thisNote=0;
                                loading = true;
                                timer = 0;
                                randomSeed(randomTimer);
                                randomTimer = 0;
                                currentEffect++;
                        }
                        else{
                                repeated=true;
                                thisNote=0;
                                loading = true;
                                timer = 0;
                                randomSeed(randomTimer);
                                randomTimer = 0;
                                currentEffect++;
                        }
                }
                switch(part) {
                case 1:
                        music_time = BTIME*4/duration1[thisNote];
                        tone(BUZZER_PIN,notes1[thisNote],2000/duration1[thisNote]);
                        break;
                case 2:
                        music_time = BTIME*4/duration2[thisNote];
                        tone(BUZZER_PIN,notes2[thisNote],2000/duration2[thisNote]);
                        break;
                }
                music_timer = 0;
        }
}

//SPI工作在MSBFIRST，即高位在前，一次一字节
void renderCube() {
        for (uint8_t i = 0; i < 8; i++) {
                digitalWrite(SS, LOW);  //SPI片选操作，发送数据前写低电平，发送后写高电品
                SPI.transfer(0x80 >> i); //层控制，接反了换成0x01<<i就OK
                for (uint8_t j = 0; j < 8; j++) { //发送面点阵
                        SPI.transfer(cube[i][j]);
                }
                digitalWrite(SS, HIGH);
        }
}

//特效Rain
void rain() {
        if (loading) {
                clearCube();
                loading = false;
        }
        timer++;
        if (timer > RAIN_TIME) {
                timer = 0;
                shift(NEG_Y);
                uint8_t numDrops = random(0, 5);
                for (uint8_t i = 0; i < numDrops; i++) {
                        setVoxel(random(0, 8), 7, random(0, 8));
                }
        }
}

//特效 PlaneBoing
uint8_t planePosition = 0;
uint8_t planeDirection = 0;
bool looped = false;

void planeBoing() {
        if (loading) {
                clearCube();
                uint8_t axis = random(0, 3);
                planePosition = random(0, 2) * 7;
                setPlane(axis, planePosition);
                if (axis == XAXIS) {
                        if (planePosition == 0) {
                                planeDirection = POS_X;
                        } else {
                                planeDirection = NEG_X;
                        }
                } else if (axis == YAXIS) {
                        if (planePosition == 0) {
                                planeDirection = POS_Y;
                        } else {
                                planeDirection = NEG_Y;
                        }
                } else if (axis == ZAXIS) {
                        if (planePosition == 0) {
                                planeDirection = POS_Z;
                        } else {
                                planeDirection = NEG_Z;
                        }
                }
                timer = 0;
                looped = false;
                loading = false;
        }

        timer++;
        if (timer > PLANE_BOING_TIME) {
                timer = 0;
                shift(planeDirection);
                if (planeDirection % 2 == 0) {
                        planePosition++;
                        if (planePosition == 7) {
                                if (looped) {
                                        loading = true;
                                } else {
                                        planeDirection++;
                                        looped = true;
                                }
                        }
                } else {
                        planePosition--;
                        if (planePosition == 0) {
                                if (looped) {
                                        loading = true;
                                } else {
                                        planeDirection--;
                                        looped = true;
                                }
                        }
                }
        }
}

//特效 sendVoxels
uint8_t selX = 0;
uint8_t selY = 0;
uint8_t selZ = 0;
uint8_t sendDirection = 0;
bool sending = false;

void sendVoxels() {
        if (loading) {
                clearCube();
                for (uint8_t x = 0; x < 8; x++) {
                        for (uint8_t z = 0; z < 8; z++) {
                                setVoxel(x, random(0, 2) * 7, z);
                        }
                }
                loading = false;
        }

        timer++;
        if (timer > SEND_VOXELS_TIME) {
                timer = 0;
                if (!sending) {
                        selX = random(0, 8);
                        selZ = random(0, 8);
                        if (getVoxel(selX, 0, selZ)) {
                                selY = 0;
                                sendDirection = POS_Y;
                        } else if (getVoxel(selX, 7, selZ)) {
                                selY = 7;
                                sendDirection = NEG_Y;
                        }
                        sending = true;
                } else {
                        if (sendDirection == POS_Y) {
                                selY++;
                                setVoxel(selX, selY, selZ);
                                clearVoxel(selX, selY - 1, selZ);
                                if (selY == 7) {
                                        sending = false;
                                }
                        } else {
                                selY--;
                                setVoxel(selX, selY, selZ);
                                clearVoxel(selX, selY + 1, selZ);
                                if (selY == 0) {
                                        sending = false;
                                }
                        }
                }
        }
}

//特效 woopWoop
uint8_t cubeSize = 0;
bool cubeExpanding = true;

void woopWoop() {
        if (loading) {
                clearCube();
                cubeSize = 2;
                cubeExpanding = true;
                loading = false;
        }

        timer++;
        if (timer > WOOP_WOOP_TIME) {
                timer = 0;
                if (cubeExpanding) {
                        cubeSize += 2;
                        if (cubeSize == 8) {
                                cubeExpanding = false;
                        }
                } else {
                        cubeSize -= 2;
                        if (cubeSize == 2) {
                                cubeExpanding = true;
                        }
                }
                clearCube();
                drawCube(4 - cubeSize / 2, 4 - cubeSize / 2, 4 - cubeSize / 2, cubeSize);
        }
}

//特效 cubeJump
uint8_t xPos;
uint8_t yPos;
uint8_t zPos;

void cubeJump() {
        if (loading) {
                clearCube();
                xPos = random(0, 2) * 7;
                yPos = random(0, 2) * 7;
                zPos = random(0, 2) * 7;
                cubeSize = 8;
                cubeExpanding = false;
                loading = false;
        }

        timer++;
        if (timer > CUBE_JUMP_TIME) {
                timer = 0;
                clearCube();
                if (xPos == 0 && yPos == 0 && zPos == 0) {
                        drawCube(xPos, yPos, zPos, cubeSize);
                } else if (xPos == 7 && yPos == 7 && zPos == 7) {
                        drawCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize);
                } else if (xPos == 7 && yPos == 0 && zPos == 0) {
                        drawCube(xPos + 1 - cubeSize, yPos, zPos, cubeSize);
                } else if (xPos == 0 && yPos == 7 && zPos == 0) {
                        drawCube(xPos, yPos + 1 - cubeSize, zPos, cubeSize);
                } else if (xPos == 0 && yPos == 0 && zPos == 7) {
                        drawCube(xPos, yPos, zPos + 1 - cubeSize, cubeSize);
                } else if (xPos == 7 && yPos == 7 && zPos == 0) {
                        drawCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos, cubeSize);
                } else if (xPos == 0 && yPos == 7 && zPos == 7) {
                        drawCube(xPos, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize);
                } else if (xPos == 7 && yPos == 0 && zPos == 7) {
                        drawCube(xPos + 1 - cubeSize, yPos, zPos + 1 - cubeSize, cubeSize);
                }
                if (cubeExpanding) {
                        cubeSize++;
                        if (cubeSize == 8) {
                                cubeExpanding = false;
                                xPos = random(0, 2) * 7;
                                yPos = random(0, 2) * 7;
                                zPos = random(0, 2) * 7;
                        }
                } else {
                        cubeSize--;
                        if (cubeSize == 1) {
                                cubeExpanding = true;
                        }
                }
        }
}

//点亮函数，灯测试
void lit() {
        if (loading) {
                clearCube();
                for (uint8_t i = 0; i < 8; i++) {
                        for (uint8_t j = 0; j < 8; j++) {
                                cube[i][j] = 0xFF;
                        }
                }
                loading = false;
        }
}

//点亮（x,y,x）上的点
void setVoxel(uint8_t x, uint8_t y, uint8_t z) {
        cube[7 - y][7 - z] |= (0x01 << x);
}
//熄灭（x,y,x）上的点
void clearVoxel(uint8_t x, uint8_t y, uint8_t z) {
        cube[7 - y][7 - z] ^= (0x01 << x);
}
//取得（x,y,x）点亮灭状态
bool getVoxel(uint8_t x, uint8_t y, uint8_t z) {
        return (cube[7 - y][7 - z] & (0x01 << x)) == (0x01 << x);
}

//点亮平面
void setPlane(uint8_t axis, uint8_t i) {
        for (uint8_t j = 0; j < 8; j++) {
                for (uint8_t k = 0; k < 8; k++) {
                        if (axis == XAXIS) {
                                setVoxel(i, j, k);
                        } else if (axis == YAXIS) {
                                setVoxel(j, i, k);
                        } else if (axis == ZAXIS) {
                                setVoxel(j, k, i);
                        }
                }
        }
}

//平移，6个方向：POS_X、NEG_X等
void shift(uint8_t dir) {

        if (dir == POS_X) {
                for (uint8_t y = 0; y < 8; y++) {
                        for (uint8_t z = 0; z < 8; z++) {
                                cube[y][z] = cube[y][z] << 1;
                        }
                }
        } else if (dir == NEG_X) {
                for (uint8_t y = 0; y < 8; y++) {
                        for (uint8_t z = 0; z < 8; z++) {
                                cube[y][z] = cube[y][z] >> 1;
                        }
                }
        } else if (dir == POS_Y) {
                for (uint8_t y = 1; y < 8; y++) {
                        for (uint8_t z = 0; z < 8; z++) {
                                cube[y - 1][z] = cube[y][z];
                        }
                }
                for (uint8_t i = 0; i < 8; i++) {
                        cube[7][i] = 0;
                }
        } else if (dir == NEG_Y) {
                for (uint8_t y = 7; y > 0; y--) {
                        for (uint8_t z = 0; z < 8; z++) {
                                cube[y][z] = cube[y - 1][z];
                        }
                }
                for (uint8_t i = 0; i < 8; i++) {
                        cube[0][i] = 0;
                }
        } else if (dir == POS_Z) {
                for (uint8_t y = 0; y < 8; y++) {
                        for (uint8_t z = 1; z < 8; z++) {
                                cube[y][z - 1] = cube[y][z];
                        }
                }
                for (uint8_t i = 0; i < 8; i++) {
                        cube[i][7] = 0;
                }
        } else if (dir == NEG_Z) {
                for (uint8_t y = 0; y < 8; y++) {
                        for (uint8_t z = 7; z > 0; z--) {
                                cube[y][z] = cube[y][z - 1];
                        }
                }
                for (uint8_t i = 0; i < 8; i++) {
                        cube[i][0] = 0;
                }
        }
}

void drawCube(uint8_t x, uint8_t y, uint8_t z, uint8_t s) {
        for (uint8_t i = 0; i < s; i++) {
                setVoxel(x, y + i, z);
                setVoxel(x + i, y, z);
                setVoxel(x, y, z + i);
                setVoxel(x + s - 1, y + i, z + s - 1);
                setVoxel(x + i, y + s - 1, z + s - 1);
                setVoxel(x + s - 1, y + s - 1, z + i);
                setVoxel(x + s - 1, y + i, z);
                setVoxel(x, y + i, z + s - 1);
                setVoxel(x + i, y + s - 1, z);
                setVoxel(x + i, y, z + s - 1);
                setVoxel(x + s - 1, y, z + i);
                setVoxel(x, y + s - 1, z + i);
        }
}

void lightCube() {
        for (uint8_t i = 0; i < 8; i++) {
                for (uint8_t j = 0; j < 8; j++) {
                        cube[i][j] = 0xFF;
                }
        }
}

void clearCube() {
        for (uint8_t i = 0; i < 8; i++) {
                for (uint8_t j = 0; j < 8; j++) {
                        cube[i][j] = 0;
                }
        }
}
