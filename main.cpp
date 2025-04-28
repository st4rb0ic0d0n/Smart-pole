#include <AccelStepper.h>

// Chân động cơ bước
#define motorY_pin1 4
#define motorY_pin2 5
#define motorY_pin3 6
#define motorY_pin4 7
#define motorX_pin1 8
#define motorX_pin2 9
#define motorX_pin3 10
#define motorX_pin4 11

// Cảm biến ánh sáng
#define X1 A1
#define X2 A2

// Hằng số
#define STEPS_PER_REV 2048
#define CM_PER_REV 2
#define STEPS_PER_CM (STEPS_PER_REV / CM_PER_REV)
#define INITIAL_MOVE_CM 5
#define SENSOR_THRESHOLD 50

AccelStepper stepperX(AccelStepper::FULL4WIRE, motorX_pin1, motorX_pin2, motorX_pin3, motorX_pin4);
AccelStepper stepperY(AccelStepper::FULL4WIRE, motorY_pin1, motorY_pin2, motorY_pin3, motorY_pin4);

enum SystemState { IDLE, MOVING_OUT, SEEKING };
SystemState state = IDLE;

long currentX = 0;
long currentY = 0;

void setup() {
    Serial.begin(9600);
    
    // Cấu hình động cơ
    stepperX.setMaxSpeed(1000);
    stepperX.setAcceleration(500);
    stepperY.setMaxSpeed(1000);
    stepperY.setAcceleration(500);

    // Bắt đầu quy trình
    state = MOVING_OUT;
}

// Hàm di chuyển động cơ đến vị trí cụ thể
bool moveMotorsTo(long targetX, long targetY) {
    stepperX.moveTo(targetX);
    stepperY.moveTo(targetY);

    stepperX.run();
    stepperY.run();

    bool xReady = (stepperX.distanceToGo() == 0);
    bool yReady = (stepperY.distanceToGo() == 0);

    if (xReady && yReady) {
        currentX = targetX;
        currentY = targetY;
        return true;
    }
    return false;
}

// Hàm tìm nguồn sáng
void seekLightSource() {
    int x1 = analogRead(X1);
    int x2 = analogRead(X2);

    if (abs(x1 - x2) > SENSOR_THRESHOLD) {
        int dirX = (x1 > x2) ? 1 : -1;
        stepperX.move(dirX * STEPS_PER_CM);
        currentX += dirX * STEPS_PER_CM;
    }
}

void loop() {
    switch (state) {
        case MOVING_OUT:
            if (moveMotorsTo(0, INITIAL_MOVE_CM * STEPS_PER_CM)) {
                state = SEEKING;
            }
            break;

        case SEEKING:
            seekLightSource();
            break;

        case IDLE:
        default:
            break;
    }
}
