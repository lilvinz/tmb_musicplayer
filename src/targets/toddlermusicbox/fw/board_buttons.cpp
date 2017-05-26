#include "board_buttons.h"
#include "target_cfg.h"


#if HAL_USE_BUTTONS

tmb_musicplayer::Button BoardButtons::BtnPlay(GPIOE, 3);
tmb_musicplayer::Button BoardButtons::BtnNext(GPIOE, 2);
tmb_musicplayer::Button BoardButtons::BtnPrev(GPIOE, 4);
tmb_musicplayer::Button BoardButtons::BtnVolUp(GPIOE, 1);
tmb_musicplayer::Button BoardButtons::BtnVolDown(GPIOE, 5);
tmb_musicplayer::Button BoardButtons::BtnCardDetect(GPIOA, 15u);

tmb_musicplayer::Button* BoardButtons::Buttons[] = {
        &BoardButtons::BtnPlay,
        &BoardButtons::BtnNext,
        &BoardButtons::BtnPrev,
        &BoardButtons::BtnVolUp,
        &BoardButtons::BtnVolDown,
        &BoardButtons::BtnCardDetect
};

#endif /* HAL_USE_BUTTONS */

