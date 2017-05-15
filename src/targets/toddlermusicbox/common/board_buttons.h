/*
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef BOARD_BUTTONS_H_
#define BOARD_BUTTONS_H_

#include "qhal.h"

#include "button.h"

/*
 * @brief   This file is used to expose drivers to the application.
 *          It should only contain the highest required level of drivers.
 *          Lower level drivers should remain hidden from application.
 */
#if HAL_USE_BUTTONS
class BoardButtons
{
public:
    static tmb_musicplayer::Button BtnPlay;
    static tmb_musicplayer::Button BtnNext;
    static tmb_musicplayer::Button BtnPrev;
    static tmb_musicplayer::Button BtnVolUp;
    static tmb_musicplayer::Button BtnVolDown;
    static tmb_musicplayer::Button BtnCardDetect;
    static tmb_musicplayer::Button* Buttons[5];
};
#endif /* HAL_USE_BUTTONS */

#endif /* BOARD_BUTTONS_H_ */
