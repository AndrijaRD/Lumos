#pragma once
#ifndef MySDL_GUI
#define MySDL_GUI

#include "../lib.h"
#include "../TextureManager/TM.h"

enum class Align {
    LEFT,
    RIGHT,
    BOTTOM,
    TOP,
    CENTER,
    DEFAULT
};

enum class CursorState {
    OUTSIDE,
    CLICKED,
    HOVERING,
    DRAGGING
};

struct PaddingRect {
    int top;
    int right;
    int bottom;
    int left;

    PaddingRect() : top(0), right(0), bottom(0), left(0) {}

    PaddingRect(int padding)
        : top(padding), right(padding), bottom(padding), left(padding) {}

    PaddingRect(int _top, int _right, int _bottom, int _left)
        : top(_top), right(_right), bottom(_bottom), left(_left) {} 
};

struct BorderRadiusRect {
    uint top_left;
    uint top_right;
    uint bottom_left;
    uint bottom_right;

    BorderRadiusRect(): 
        top_left(0), 
        top_right(0), 
        bottom_left(0), 
        bottom_right(0) {};

    BorderRadiusRect(uint radius): 
        top_left(radius), 
        top_right(radius), 
        bottom_left(radius), 
        bottom_right(radius) {};
    
    BorderRadiusRect(
        uint tl, // top-left
        uint tr, // top-right
        uint bl, // bottom-left
        uint br // bottom-right
    ): 
        top_left(tl), 
        top_right(tr), 
        bottom_left(bl), 
        bottom_right(br) {};
};





/**
 * @brief Helper function, used in Rect and Line GUI elements.
 * 
 * @param p1 SDL_Point
 * @param p2 SDL_Point
 * @param color SDL_Color
 * @param thickness int
 */
void drawThickLineSegment(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color, int thickness);



class GUI{
    friend class Sys;

    friend bool isAbsolutePointInContainerRect(SDL_Point point, SDL_Rect rect);
    friend void drawThickLineSegment(
        const SDL_Point& p1, 
        const SDL_Point& p2, 
        const SDL_Color& color, 
        int thickness
    );

private:
    /**
     * @brief LoadedText structure, used to store some commonly re-used texts
     * 
     * Insted of recompiling text textures every single frame they are stored
     * in a unordered_map<LoadedText> which has max capacity of MAX_LOADED_TEXTS 
     * 
     * Whenever user requests to render a text, first the GUI::Text function
     * checks if there are any pre-loaded, pre-used, text textures matching
     * the requested text, using the textID, which is calculated with title, 
     * fontSize and color of the text. 
     * 
     * If there are no matching ids, the new LoadedText object is pushed into 
     * the unordered map.
     * 
     * If the unordered map has length greater then MAX_LOADED_TEXTS, 
     * the texture that was the least used gets removed. 
     * 
     * To determene which one is the least used, library
     * looks at lastUsedFrame variable. The olderst is deleted.
     */
    struct LoadedText {
        TextureData td;     // Holds the Compiled Text Texture
        string title;       // The text
        int fontSize;       // The fontSize used
        SDL_Color color;    // The color of text
        int lastUsedFrame;  // Last frame that this texture was used

        // Defualt Initializor
        LoadedText(
            TextureData _td = TextureData(),
            string _title = "",
            int _fontSize = 0,
            SDL_Color _color = SDL_COLOR_WHITE,
            int _lastUsedFrame = 0
        ):
            td(_td),
            title(_title),
            fontSize(_fontSize),
            color(_color),
            lastUsedFrame(_lastUsedFrame) {};

        // Calculate the TextID
        string getId() { return getTextId(title, fontSize, color); }

        // Comparator: sort by value
        bool operator<(const LoadedText& other) const { return lastUsedFrame < other.lastUsedFrame; }
        bool operator>(const LoadedText& other) const { return lastUsedFrame > other.lastUsedFrame; }
    };

    // Unordered Map for storing Loaded Texts
    // Key is text id, obtained from getTextId() or LoadedText.getId()
    static inline unordered_map<string, LoadedText> loadedTexts;
    static inline int MAX_LOADED_TEXTS = 0; // 0 means unlimited

    static string getTextId(const string& title, int fontSize, SDL_Color color);
    static LoadedText* loadNewText(string title, int fontSize, SDL_Color color);
    static void removeOldestText();





    /**
     * @brief InputState structure, used for storing the current state
     * of a single Input Field. They are all stored in a unordered_map
     * which allows O(1) access, using the key, which is the unique id
     * of each input given by the user, GUI::Input(string id, ...).
     * 
     * Unlike the LoadedTexts there is no limit on how many input fields
     * there can be, and so the states wont self destruct, if you want to
     * destroy or reset some state you do so by using GUI::DestroyInput
     * and passing the unique id to it. If the input with the same id
     * gets called again, it will be created again, from scratch, so
     * it can be used to reset some filed too. 
     */
    struct InputState {
        string id;          // Holds the uniqueId
        string value;       // Holds the current value of the Input Field
        TextureData td;  // Holds the texture of the Text

        bool focused;   // Keeps track of if the field is focused
                        // If it is, capture the keyboard strokes and add them to value
        
        bool firstRender = true;    // Used to check if it the first time that
                                    // the input (with this id) has been rendered
                                    // usefull when determining auto focus

        bool change = true;     // Keeps track of if the value has been changed
                                // and texture needs to be reCompiled, by default
                                // its true, so the texture would get compiled the first Frame
        
        bool deleting = false;  // Used to check if the backspace has been down
        
        int removed = 0;    // Keeps the track of how many character need to be cut 
                            // from the left, so the text can be fit inside the field
        
        
        // Default Constructor
        InputState(
            const string& _id, 
            const string& _value = "",
            const bool& _focused = false
        ): 
            id(_id),
            value(_value),
            focused(_focused) {};

        // << Overload, used for debugging
        friend ostream& operator<<(ostream& os, const InputState& state);
    };

    // Unordered Map for storing Input States
    // Key is InputState.id
    static inline unordered_map<string, InputState> inputStates;






    struct ContainerState {
        string id;
        SDL_Rect dRect;
        int contentHeight = 0;

        int scrollOffset = 0;
        int scrollSpeed = 20;

        // For scrollbar dragging:
        bool scrollbarDragging = false;
        int scrollbarDragStartMouseY = 0;
        int scrollbarDragStartScrollOffset = 0;

        int lastActiveFrame = 0;

        // Default Constructor
        ContainerState(string _id){
            id = _id;
        }
    };

    static inline unordered_map<string, ContainerState> containerStates;
    static inline string activeContainer;

    static void renderVerticalScrollbar(
        const SDL_Rect&   container,      // x,y,w,h of the visible area
        int               contentHeight, // total height of scrollable content
        int&              scrollOffset   // current scroll offset—in/out
    );


    /**
     * @brief Internal helper function called by GUI::Rect
     * 
     * @param dRect SDL_Rect dRect
     * @param color SDL_Color color
     * @param thickness int thickenss
     */
    static void renderRect(
        const SDL_Rect& dRect,
        const SDL_Color& color,
        const int thickness,
        const BorderRadiusRect& borderRadius
    );





    // Pushable States ----------------------------------------
    static inline int pFontSize = -1;

    static inline Align pTextAlignY = Align::DEFAULT;
    static inline Align pTextAlignX = Align::DEFAULT; 

    static inline bool pAutoFocus = false;
    static inline bool pInputLock = false;
    static inline string pDefaultValue = "";
    static inline int pOutlineThickness = -1;
    static inline SDL_Color pOutlineColor = {0, 0, 0, 0};

    static inline BorderRadiusRect pBorderRadius = {0, 0, 0, 0};
    static inline PaddingRect pPaddingRect = {-1, -1, -1, -1};

    static inline bool pDashLine = false;
    static inline int pDashSize = 0;
    static inline int pDashGapSize = 0;
    

public:

    /** GUI Text
     * 
     * Draws a optimized verion of text on the screen.
     * For styling it supports:
     *      /
     * 
     * === === === === === === === === === === === === === === ===
     *      It doesnt respond to any push styles since all
     *      of them can be controlled trough dRect param. 
     * === === === === === === === === === === === === === === ===
     * 
     * @param text Its the text that will be rendered on the screen
     * @param dRect The coordinates and size of the text, Height is
     *              actually the font size in which text will be rendered.
     *              Either width or height can be left to be -1, undefined,
     *              meaning they will be automaticly calculated using the
     *              aspect ratio of the text. THE ORIGINAL WILL BE UPDATED!
     * @param color Color of the text expressed in SDL_Color
     */
    static void Text(
        const string& text, 
        SDL_Rect& dRect, 
        const SDL_Color& color
    );

    /** Text Dynamic
     * 
     * Draws a text that is not cached anywhere, the texture is created,
     * rendered and then destroyed, its not optimal, but it shold be used for 
     * highly changable texts, like the ones that change every frame so there
     * is no point is storing them as they will never be reused unline regular
     * GUI::Text which is optimised so it stores all texture in case of re-use.
     * 
     * @param title Text to be rendered.
     * @param dRect position and size of the text, one dimension can be left as -1 for
     *              dynamic calculation, eg. set height to font size and width as -1
     *              and after the render you can check dRect that was passed and the
     *              width will be calculated and dRect will be updated, replacing -1
     * @param color The color of the text
     * 
     */
    static void TextDynamic(
        const string& title, 
        SDL_Rect& dRect, 
        const SDL_Color& color
    );

    /**
     * @brief Calculates the Width of the text using its aspect ratio.
     * 
     * @param text The text
     * @param textHeight The font size
     * @return int 
     */
    static int calcTextWidth(const string& text, int textHeight);

    /**
     * @brief Calculates the Height (fontSize) of the text using its aspect ratio.
     * 
     * @param text The text
     * @param textWidth The width that text will occupy
     * @return int 
     */
    static int calcTextHeight(const string& text, int textWidth);

    static void setMaxNumOfLoadedTextures(int number = 0);


    /** GUI Button
     * 
     * This function renders a button.
     * For styling it supports:
     *      - pushFontSize
     *      - pushTextAlignX
     *      - pushTextAlignY
     *      - pushBorderRadius
     *      - pushPadding
     *      - pushLineStyle
     * 
     * @param title Button text, which is dinamicly calculated to fit into the button and to be centered
     * @param dRect It represents position and size of the button, must be possitive, all of the value
     * @param buttonColor Background color of the button
     * @param textColor Color of the text inside the Button
     * 
     * @returns It returns the state of the button: (GuiCursorState)
     *  `CURSOR_CLICKED`, 
     *  `CURSOR_HOVERING`,
     *  `CUROSR_DRAGGING`,
     *  `CURSOR_OUTSIDE`
     * 
     */
    static CursorState Button(
        const string& title,
        const SDL_Rect& dRect,
        const SDL_Color& buttonColor = THEME_COLOR_2,
        const SDL_Color& textColor = SDL_COLOR_WHITE
    );


    /** GUI Image
     * 
     * This function renders a SDL_Texture*.
     * For styling it supports:
     *      - pushBorderRadius
     * 
     * @param td SDL_Texture*, texture to be rendered
     * @param dRect Destination Rectangle: x, y, width, height
    */
    static void Image(
        SDL_Texture* texture,
        SDL_Rect& rect
    );

    /** GUI Image
     * 
     * This function renders a TextureData.
     * 
     * @param td TextureData, texture to be rendered
     * @param dRect Destination Rectangle: {x, y, width, height}
    */
    static void Image(
        TextureData& td,
        SDL_Rect& rect
    );

    /** GUI Image
     * 
     * This function renders an Icon.
     * 
     * @param td SVGIcon, icon to be rendered
     * @param dRect Destination Rectangle: {x, y, width, -}
    */
    static void Image(
        SVGIcon& icon,
        SDL_Rect& rect
    );


    /**
     * @brief Renders an input field with a unique identifier.
     *
     * This function displays an input field at the specified location. The `uniqueId` parameter
     * is used to maintain the state of the input field across different frames. It is crucial
     * that each input field has a unique identifier to prevent state conflicts.
     *
     * Styling options supported:
     * - `pushFontSize`
     * - `pushTextAlignX`
     * - `pushTextAlignY`
     * - `pushAutoFocus`
     * - `pushInputLock`
     * - `pushDefaultValue`
     * - `pushBorderRadius`
     * - `pushOutlineStyle`
     * - `pushPadding`
     * - `pushLineStyle`
     *
     * @param uniqueId A unique string used internally to identify the input field across different frames.
     * @param dRect An SDL_Rect representing the position and size where the field should be rendered.
     * @param placeholder Text displayed in the input field when it is empty. Defaults to "Type text here...".
     * @param background SDL_Color representing the background color of the field. Defaults to white.
     * @param textColor SDL_Color representing the text color. Defaults to black.
     * @return The current value of the input field.
     */
    static string Input(
        const std::string& uniqueId,
        const SDL_Rect& dRect,
        const std::string& placeholder = "Type text here...",
        const SDL_Color& background = SDL_COLOR_WHITE,
        const SDL_Color& textColor = SDL_COLOR_BLACK
    );

    /**
     * @brief Destroys the InputState object, removes it from unordered map
     * 
     * @param inputId The ID of the input to be destroyed, reset
     */
    static void DestroyInput(const string& inputId);

    /**
     * @brief Get the Input State object using its ID
     * 
     * @param inputId The Input's ID
     * @return InputState* 
     */
    static InputState* getInputState(const string& inputId);


    /** GUI Container
     * @brief Creates a scrollable Container
     * All of the coordinates are rest to (0, 0) meaning
     * that now rendering a SDL_Point(0, 0) would corespond
     * to the begning of the continer, the coordinates become
     * relative to the containers beginning.
     * 
     * @param uniqueId The id of the Container
     * @param dRect The position where the container should be rendered
     * @param contentMaxHeight The total height of the containers content
     */
    static void beginContainer(
        const string& uniqueId,
        const SDL_Rect& dRect
    );

    /** GUI Container
     * @brief Marks the end of the container.
     */
    static void endContainer();

    /**
     * @brief Get the Container State object, if it doesn't exist
     * it CREATES it and then returns pointer to that state.
     * 
     * @param containerId If state doesnt exist it gets created
     * @return ContainerState*
     */
    static ContainerState* getContainerState(const string& containerId);

    static bool isRectVisible(SDL_Rect rect);




    static void Rect(
        const SDL_Rect& dRect,
        const SDL_Color& color = SDL_COLOR_WHITE,
        const int thickness = -1
    );


    static void Line(
        const SDL_Point& p1,
        const SDL_Point& p2,
        const SDL_Color& color,
        const uint& thickness = 1
    );

    static void Circle(
        const SDL_Point& center, 
        const int& radius, 
        const SDL_Color& color = SDL_COLOR_WHITE, 
        const int& thickness = -1
    );


    // Pushing the styles -------------------------------------------------------------------------
    
    static void pushFontSize(uint fontSize);

    static void pushTextAlignY(Align direction);
    static void pushTextAlignX(Align direction);

    static void pushAutoFocus();
    static void pushInputLock();

    static void pushBorderRadius(uint radius);
    static void pushBorderRadius(BorderRadiusRect radiusRect);
    static void pushBorderRadius(
        uint topLeft,
        uint topRight,
        uint bottomLeft,
        uint bottomRight
    );

    static void pushDefaultValue(string value);
    static void setInputValue(string inputId, string newValue); // Override the current value

    static void pushOutlineStyle(int thickness = -1, SDL_Color color = {0, 0, 0, 0});

    static void pushPadding(int padding);
    static void pushPadding(PaddingRect paddingRect);

    static void pushDashLineStyle(int dashLineSize, int dashGapSize);
};




#endif