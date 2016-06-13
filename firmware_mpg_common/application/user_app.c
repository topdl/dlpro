/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_CursorPosition;
static u8 u8TransMessage[8]={00,00,00,00,00,00,00,00};/*message of signal to control airconditional*/
u8 u8on[]="on";
u8 u8off[]="off";
bool boolcallonce=TRUE;
u8 u8Funtion1Message[]="switch funtio temper";
u8 u8Funtion2Message[]="wind auto sleep";
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR,u8Funtion1Message);
  LCDMessage(LINE2_START_ADDR,u8Funtion2Message);
  LCDCommand(LCD_HOME_CMD);
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
  UserApp_CursorPosition = LINE1_START_ADDR;

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  static u8 u8tempcount=20;
  while(boolcallonce)
  {
   LCDCommand(LCD_CLEAR_CMD);
   LCDMessage(LINE1_START_ADDR,u8Funtion1Message);
   LCDMessage(LINE2_START_ADDR,u8Funtion2Message);
   if(u8TransMessage[0]=0x00)
   {
   LCDMessage(LINE2_START_ADDR+16,u8off);
   }
   else
   {
    LCDMessage(LINE2_START_ADDR+16,u8on);
   }
  LCDCommand(LCD_HOME_CMD);
  boolcallonce=FALSE;
  }
  u8tempcount--;
  if(u8tempcount==0)
  {
  //button 1  move cursor forward ------------------------------------------
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    /* Handle the two special cases or just the regular case */
    if(UserApp_CursorPosition == (LINE1_END_ADDR-5))
    {
      UserApp_CursorPosition = LINE2_START_ADDR;
    }

    else if (UserApp_CursorPosition == (LINE2_END_ADDR-9))
    {
      UserApp_CursorPosition = LINE1_START_ADDR;
    }
    
    /* Otherwise just increment one space */
    else
    {
      if(UserApp_CursorPosition>=0x40)
      {
        UserApp_CursorPosition+=5;
      }
      else
      {
      UserApp_CursorPosition+=7;
      }
    }
     LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition);
  }
    //button 0 move cursor left ---------------------------------------------
    if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    /* Handle the two special cases or just the regular case */
    if(UserApp_CursorPosition == LINE1_START_ADDR)
    {
      UserApp_CursorPosition = LINE2_END_ADDR-9;
    }

    else if (UserApp_CursorPosition == LINE2_START_ADDR)
    {
      UserApp_CursorPosition = LINE1_END_ADDR-5;
    }
    
    /*cursor move*/
    else
    {
      if(UserApp_CursorPosition>=0x40)
      {
        UserApp_CursorPosition-=5;
      }
      else
      {
      UserApp_CursorPosition-=7;
      }
    }
     LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition);
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    { 
      switch(UserApp_CursorPosition)
      {
        case 0x00:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_SwitchSelect;         
        break;
        case 0x07:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_FuntionhSelect;          
        break;
        case 0x0e:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_TemperSelect;        
        break;
        case 0x40:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_WindSelect;         
        break;
        case 0x45:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_AutoSelect;       
        break;
        case 0x4a:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_SleepSelect;
        break;
      }
    }
  }
  u8tempcount=20;
  }
} /* end UserAppSM_Idle() */

/*funtion select -------------------------------------- */    
static void UserAppSM_SwitchSelect(void)
{
  u8TransMessage[0]=~u8TransMessage[0];
  boolcallonce=TRUE;
  UserApp_StateMachine = UserAppSM_Idle;
  
}
/*------------------------------------------------------*/
static void UserAppSM_FuntionhSelect(void)
{
  static u8 u8Funtion1SubMenu[]="cool heat comf ";
  static u8 u8Funtion2SubMenu[]="arefaction aeration";
  while(boolcallonce)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR,u8Funtion1SubMenu);
    LCDMessage(LINE2_START_ADDR,u8Funtion2SubMenu);
    boolcallonce=FALSE;
    LCDCommand(LCD_HOME_CMD);  
    UserApp_CursorPosition=0x00;
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    /* Handle the two special cases or just the regular case */
    if(UserApp_CursorPosition == (LINE1_END_ADDR-9))
    {
      UserApp_CursorPosition = LINE2_START_ADDR;
    }

    else if (UserApp_CursorPosition == (LINE2_END_ADDR-8))
    {
      UserApp_CursorPosition = LINE1_START_ADDR;
    }
    
    /* Otherwise just increment one space */
    else
    {
      if(UserApp_CursorPosition>=0x40)
      {
        UserApp_CursorPosition+=11;
      }
      else
      {
      UserApp_CursorPosition+=5;
      }
    }
     LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition);
  }
    //button 0 move cursor left ---------------------------------------------
    if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    /* Handle the two special cases or just the regular case */
    if(UserApp_CursorPosition == LINE1_START_ADDR)
    {
      UserApp_CursorPosition = LINE2_END_ADDR-8;
    }

    else if (UserApp_CursorPosition == LINE2_START_ADDR)
    {
      UserApp_CursorPosition = LINE1_END_ADDR-9;
    }
    
    /* Otherwise just increment one space */
    else
    {
      if(UserApp_CursorPosition>=0x40)
      {
        UserApp_CursorPosition-=11;
      }
      else
      {
      UserApp_CursorPosition-=5;
      }
    }
     LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition);
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    {
      switch(UserApp_CursorPosition)
      {
        case 0x00:
          u8TransMessage[1]=00;
          break;
        case 0x05:
          u8TransMessage[1]=01;
          break;
        case 0x0a:
          u8TransMessage[1]=02;
          break;
        case 0x40:
          u8TransMessage[1]=03;
          break;
        case 0x4b:
          u8TransMessage[1]=04;
          break;
      }
      boolcallonce=TRUE;
      UserApp_StateMachine = UserAppSM_Idle;
    }
  }
}
/*------------------------------------------------------*/
static void UserAppSM_TemperSelect(void)
{
  
}
/*------------------------------------------------------*/
static void UserAppSM_WindSelect(void)
{
  
}
/*------------------------------------------------------*/
static void UserAppSM_AutoSelect(void)
{
  
}
/*------------------------------------------------------*/
static void UserAppSM_SleepSelect(void)
{
  
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
