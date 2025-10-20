struct VPS_Rect
{
    struct VPS_Point position;
    struct VPS_Size size;
};

char VPS_Rect_Init(struct VPS_Rect *rect, struct VPS_Point position, struct VPS_Size size);