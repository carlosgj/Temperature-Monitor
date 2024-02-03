#include "UI.h"

uint8_t UI_init(void) {
    uint8_t status = 0;

    //Set up buttons
    BUTTON1_TRIS = INPUT;
    BUTTON2_TRIS = INPUT;
    BUTTON3_TRIS = INPUT;
    BUTTON4_TRIS = INPUT;
    BUTTON5_TRIS = INPUT;
    BUTTON6_TRIS = INPUT;

    BUTTON1_WPU = TRUE;
    BUTTON2_WPU = TRUE;
    BUTTON3_WPU = TRUE;
    BUTTON4_WPU = TRUE;
    BUTTON5_WPU = TRUE;
    BUTTON6_WPU = TRUE;

    buttonState.all = 0;
    oldButtonState.all = 0;
    buttonPressed.all = 0;

    return status;
}

void UI_periodic(uint8_t minorCycle) {
    drawTemp();
    drawTime();
    updateButtons();
    handleButtonActions();
    if (safeMode) {
        //drawSafeMode();
    }
    if (currentDisplayMode == DISP_MODE_HOME) {
        switch (currentGraphMode) {
            case GRAPH_MODE_DAY:
                switch (minorCycle) {
                    case 0:
                        clearPlotXLabels();
                        break;
                    case 1:
                        drawPlotDayXLabels();
                        break;
                    case 2:
                        //collectDayData();
                        break;
                    case 3:
                        plotTemp(240);
                        break;
                    default:
                        break;
                }
                break;
            case GRAPH_MODE_WEEK:
                switch (minorCycle) {
                    case 0:
                        clearPlotXLabels();
                        break;
                    case 1:
                        drawPlotWeekXLabels();
                        break;
                    case 2:
                        //collectWeekData();
                        break;
                    case 3:
                        plotTemp(336);
                        break;
                    default:
                        break;
                }
                break;
            case GRAPH_MODE_MONTH:
                //clearPlotXLabels();
                //drawPlotMonthXLabels();
                //collectMonthData();
                //plotTemp(360);
                break;
            case GRAPH_MODE_YEAR:
                //clearPlotXLabels();
                //drawPlotYearXLabels();
                //collectYearData();
                //plotTemp(365);
                break;

        }
    }

}

void updateButtons(void) {
    oldButtonState.all = buttonState.all;

    //    buttonState.B1 = !BUTTON1_PORT;
    //    buttonState.B2 = !BUTTON2_PORT;
    //    buttonState.B3 = !BUTTON3_PORT;
    //    buttonState.B4 = !BUTTON4_PORT;
    //    buttonState.B5 = !BUTTON5_PORT;
    //    buttonState.B6 = !BUTTON6_PORT;

    buttonPressed.all = buttonState.all & ~oldButtonState.all;
}

void handleButtonActions(void) {
    //If any button is pressed when asleep, wake up
    if (buttonPressed.all != 0 && isSleep) {
        setSleep(FALSE);
        __delay_ms(250);
        return;
    }
    if (buttonPressed.B1) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                setGraphMode(GRAPH_MODE_DAY);
                break;
            case DISP_MODE_UTIL:
                //Switch to home
                setDisplayMode(DISP_MODE_HOME);
                break;
            case DISP_MODE_SETTIME:
                //Switch to home
                setDisplayMode(DISP_MODE_HOME);
                break;
        }
        return;
    }

    if (buttonPressed.B2) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Switch to past-week plot
                setGraphMode(GRAPH_MODE_WEEK);
                break;
            case DISP_MODE_UTIL:
                //Switch to time-set screen
                setDisplayMode(DISP_MODE_SETTIME);
                //getTime();
                pros_seconds.all = seconds_reg.all;
                pros_minutes.all = minutes_reg.all;
                pros_hours.all = hours_reg.all;
                pros_date.all = date_reg.all;
                pros_month.all = month_reg.all;
                pros_years.all = years_reg.all;
                activeTimeChar = ACTIVE_TIME_10YR;
                drawProspectiveTime();
                break;
            case DISP_MODE_SETTIME:
                //setTime();
                setDisplayMode(DISP_MODE_HOME);
                break;
        }
        return;
    }

    if (buttonPressed.B3) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Switch to past-month plot
                setGraphMode(GRAPH_MODE_MONTH);
                break;
            case DISP_MODE_UTIL:
                //Force start RTC
                //RTCOscRestart();
                break;
            case DISP_MODE_SETTIME:
                //Move cursor left
                if (activeTimeChar == 0) {
                    activeTimeChar = ACTIVE_TIME_SECOND;
                } else {
                    activeTimeChar--;
                }
                drawProspectiveTime();
                break;
            case DISP_MODE_VERIFY_RESET:
                //"No" selected
                setDisplayMode(DISP_MODE_HOME);
                break;
        }
        return;
    }

    if (buttonPressed.B4) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Switch to past-year plot
                setGraphMode(GRAPH_MODE_YEAR);
                break;
            case DISP_MODE_UTIL:
                //Reset
                RESET();
                break;
            case DISP_MODE_SETTIME:
                //Move cursor right
                if (activeTimeChar == ACTIVE_TIME_SECOND) {
                    activeTimeChar = 0;
                } else {
                    activeTimeChar++;
                }
                drawProspectiveTime();
                break;
            case DISP_MODE_VERIFY_RESET:
                //"Yes" selected
                safeMode = FALSE;
                //resetEEPROMPageIndex();
                RESET();
                break;
        }
        return;
    }

    if (buttonPressed.B5) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Go to util screen
                setDisplayMode(DISP_MODE_UTIL);
                break;
            case DISP_MODE_UTIL:
                break;
            case DISP_MODE_SETTIME:
                //Increment
                incrementActiveChar();
                drawProspectiveTime();
                break;
        }
        return;
    }

    if (buttonPressed.B6) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Turn off screen
                setSleep(TRUE);
                break;
            case DISP_MODE_UTIL:
                setDisplayMode(DISP_MODE_VERIFY_RESET);
                break;
            case DISP_MODE_SETTIME:
                //Decrement
                decrementActiveChar();
                drawProspectiveTime();
                break;
        }
        return;
    }

    //If a button was pressed, do the action, then delay to ignore bounces
    if (buttonPressed.all != 0) {
        __delay_ms(300);
    }
}