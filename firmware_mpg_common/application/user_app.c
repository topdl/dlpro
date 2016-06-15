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

extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_CursorPosition;
static u8 u8TransMessage[8]={0xff,00,16,01,00,0xff,00,00};/*message of signal to control airconditional*/
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
  /* Configure ANT for this application */
  G_stAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  G_stAntSetupData.AntSerialLo         = ANT_SERIAL_LO_USERAPP;
  G_stAntSetupData.AntSerialHi         = ANT_SERIAL_HI_USERAPP;
  G_stAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  G_stAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  G_stAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  G_stAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  G_stAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  G_stAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  LCDCommand(LCD_HOME_CMD);
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
  UserApp_CursorPosition = LINE1_START_ADDR;

  /* If good initialization, set state to Idle */
  if( AntChannelConfig(ANT_MASTER) )
  {
    AntOpenChannel();
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
static void UserAppSM_Idle(void)
{
  //idle state initialization 
  while(boolcallonce)
  {
   UserApp_CursorPosition = LINE1_START_ADDR; 
   LCDCommand(LCD_CLEAR_CMD);
   LCDMessage(LINE1_START_ADDR,u8Funtion1Message);
   LCDMessage(LINE2_START_ADDR,u8Funtion2Message);
   if(u8TransMessage[0]==0xff)
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
    
    /* Otherwise just increment  */
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
  /*button2 confirm the selection*/
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    { 
      /*enter different modules*/
      switch(UserApp_CursorPosition)
      {
        case 0x00:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_SwitchSelect;         
        break;
        case 0x07:
          boolcallonce=TRUE;
          UserApp_StateMachine = UserAppSM_FuntionSelect;          
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
  /*transmit message to slave*/
  if( AntReadData() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* We got some data */
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
     /* A channel period has gone by: typically this is when new data should be queued to be sent */
      AntQueueBroadcastMessage(u8TransMessage);
    }
  } /* end AntReadData() */

} /* end UserAppSM_Idle() */

/*  switch moudle---------------------------------------*/
/*----------------------------------------------------- */    
static void UserAppSM_SwitchSelect(void)
{
  
  u8TransMessage[0]=~u8TransMessage[0];
  boolcallonce=TRUE;
  UserApp_StateMachine = UserAppSM_Idle;
  
}

/*funtion select moudle---------------------------------*/
/*------------------------------------------------------*/
static void UserAppSM_FuntionSelect(void)
{
  static u8 u8Funtion1SubMenu[]="cool heat comf ";
  static u8 u8Funtion2SubMenu[]="arefaction aeration";
  /*moudle initialize*/
  while(boolcallonce)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR,u8Funtion1SubMenu);
    LCDMessage(LINE2_START_ADDR,u8Funtion2SubMenu);
    boolcallonce=FALSE;
    LCDCommand(LCD_HOME_CMD);  
    UserApp_CursorPosition=0x00;
  }
  /*move cursor to select different funtions---------------------
  and button1 moves cursor to right button2 moves cursor to left*/
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
    /*the length of cursor moved decided by dinfferent strings*/
    else
    {
      //the cursor is in the second line
      if(UserApp_CursorPosition>=0x40)
      {
        UserApp_CursorPosition+=11;
      }
      //the cursor is in first line
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
    
    /* Otherwise just increment  */
    else
    {
      //the cursor is in the second line
      if(UserApp_CursorPosition>=0x40)
      {
        UserApp_CursorPosition-=11;
      }
      //the cursor is in first line
      else
      {
      UserApp_CursorPosition-=5;
      }
    }
     LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition);
  }
  /*confirm the selection and change the message */
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    {
      switch(UserApp_CursorPosition)
      {
        //decide the message content on the basis of cursor position
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
}/*end moudle*/

/*temprature adjust*/
/*------------------------------------------------------*/
static void UserAppSM_TemperSelect(void)
{ 

  u8 u8TMessage[]="temprature: ";
  //buffer to save temperature
  static u8 u8TCounterMessage[3]={'1','6','\0'};
  //initialization
  if(boolcallonce)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR,u8TMessage);
    LCDMessage(LINE2_START_ADDR,u8TCounterMessage);
    boolcallonce=FALSE;
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1); 
    u8TCounterMessage[1]++;
    if(u8TCounterMessage[1]==0x3a)
    {
      u8TCounterMessage[1]='0';
      u8TCounterMessage[0]++;
    }
    LCDMessage(LINE2_START_ADDR,u8TCounterMessage);
  }
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    u8TCounterMessage[1]--;
    if(u8TCounterMessage[1]==0x2f)
    {
      u8TCounterMessage[1]=0x39;
      u8TCounterMessage[0]--;
    }
    LCDMessage(LINE2_START_ADDR,u8TCounterMessage);
  }
  //confirm order
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
 //trsns temprature
    u8TransMessage[2]=(u8TCounterMessage[0]-0x30)*10+u8TCounterMessage[1]-0x30;
    boolcallonce=TRUE;
    UserApp_StateMachine = UserAppSM_Idle;
  }

}/*end moudle */


/*windspeed moudle*/
/*------------------------------------------------------*/
static void UserAppSM_WindSelect(void)
{
  u8 u8WMessage[]="windspeed :";
  //buffer to save windspeed
 static u8 u8WSpeed[]="1";
  while(boolcallonce)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR,u8WMessage);
    LCDMessage(LINE2_START_ADDR,u8WSpeed);
    boolcallonce=FALSE;
  }
  /*button0 and 1 to change windspeed ----------------
  button0 raise windspeed and button1 reduce windspeed
  the speed include 1,2,3*/
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    if(u8WSpeed[0]==0x33)
    {
      u8WSpeed[0]=0x31;
    }
    else
    {
    u8WSpeed[0]++;
    }
    LCDMessage(LINE2_START_ADDR,u8WSpeed);
  }
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    if(u8WSpeed[0]==0x31)
    {
      u8WSpeed[0]=0x33;
    }
    else
    {
      u8WSpeed[0]--;
    }
    LCDMessage(LINE2_START_ADDR,u8WSpeed);
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    {
      u8TransMessage[3]=u8WSpeed[0];
      boolcallonce=TRUE;
      UserApp_StateMachine = UserAppSM_Idle;
    }
  }
}/*end moudle*/

/*timer moudle*/
/*------------------------------------------------------*/
static void UserAppSM_AutoSelect(void)
{
  u8 u8timing[]="timing:";
  static u8 u8timelast[3]={'0','0','\0'};
  if(boolcallonce)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR,u8timing);
    LCDMessage(LINE2_START_ADDR,u8timelast);
    boolcallonce=FALSE;
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1); 
    u8timelast[1]++;
    if(u8timelast[1]==0x3a)
    {
      u8timelast[1]='0';
      u8timelast[0]++;
    }
    LCDMessage(LINE2_START_ADDR,u8timelast);
  }
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    u8timelast[1]--;
    if(u8timelast[1]==0x2f)
    {
      u8timelast[1]=0x39;
      u8timelast[0]--;
    }
    LCDMessage(LINE2_START_ADDR,u8timelast);
  }
 if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    {
      u8TransMessage[4]=(u8timelast[0]-0x30)*10+u8timelast[1]-0x30;;
      boolcallonce=TRUE;
      UserApp_StateMachine = UserAppSM_Idle;
    }
  } 
}/*end moudle*/

/*turn off all the light */
/*------------------------------------------------------*/
static void UserAppSM_SleepSelect(void)
{
  u8TransMessage[5]=~u8TransMessage[5];
   LedToggle(LCD_RED);
   LedToggle(LCD_GREEN);
   LedToggle(LCD_BLUE);
  boolcallonce=TRUE;
  UserApp_StateMachine = UserAppSM_Idle;
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
