// Simulation.hh
#ifndef SIMULATION_HH
#define SIMULATION_HH

#include "../include/Camera.hh"

class Simulation {
    public:
        // --- Class constructor and destructor ---
        Simulation(Camera* camera = nullptr);
        ~Simulation();

        // ================================================================
        // Simulation:
        // ================================================================
        void updateSimulation();

        // --- Glut timer function wrapper ---
        static void glutTimerCallback(int value) { if (instance != nullptr) {instance->updateSimulation(); glutTimerFunc(TIMER, glutTimerCallback, value);}};

        // ================================================================
        // Keyboard & mouse handling:
        // ================================================================
        void keyboardControl(unsigned char key, int x, int y);
        void keyboardControlUp(unsigned char key, int x, int y);
        void mouseControl(int button, int state, int x, int y);
        void mouseMotionControl(int x, int y);
        
        // --- Static callbacks for Glut ---
        static void glutKeyboardCallback(unsigned char key, int x, int y) { if (instance != nullptr) instance->keyboardControl(key, x, y); };
        static void glutKeyboardUpCallback(unsigned char key, int x, int y) { if (instance != nullptr) instance->keyboardControlUp(key, x, y); };
        static void glutMouseCallback(int button, int state, int x, int y) {if (instance != nullptr) instance->mouseControl(button, state, x, y); };
        static void glutMouseMotionControl(int x, int y) { if (instance != nullptr) instance->mouseMotionControl(x, y); };

    private:
        // ================================================================
        // Constants:
        // ================================================================
        static constexpr int TIMER = 16; // in milliseconds (16 ms = ~60 FPS)

        // ================================================================
        // Class object-pointers:
        // ================================================================
        static Simulation* instance; 
        Camera* camera;

        // ================================================================
        // Keyboard & mouse handling variables:
        // ================================================================
        bool keyStates[256];
        int lastMouseX;
        int lastMouseY;
        bool isMousePressed;
        bool isShiftPressed;

        // ================================================================
        // Simulation speed control variables:
        // ================================================================
        int previousTime;
        double deltaTime; // Elapsed time in seconds (ex: 0.016 = 60 FPS)
};

#endif
