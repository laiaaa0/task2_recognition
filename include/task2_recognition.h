// Copyright (C) 2010-2011 Institut de Robotica i Informatica Industrial, CSIC-UPC.
// Author
// All rights reserved.
//
// This file is part of iri-ros-pkg
// iri-ros-pkg is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// IMPORTANT NOTE: This code has been generated through a script from the
// iri_ros_scripts. Please do NOT delete any comments to guarantee the correctness
// of the scripts. ROS topics can be easly add by using those scripts. Please
// refer to the IRI wiki page for more information:
// http://wikiri.upc.es/index.php/Robotics_Lab

#ifndef _task2_recognition_module_h_
#define _task2_recognition_module_h_


//IRI ROS headers
#include <iri_ros_tools/module.h>
#include <iri_ros_tools/module_action.h>
#include <iri_ros_tools/module_service.h>
#include <iri_ros_tools/module_dyn_reconf.h>
#include <iri_ros_tools/watchdog.h>
#include <iri_ros_tools/timeout.h>

#include <erl_classification_modules/face_recognition_module.h>
#include <erl_classification_modules/shirt_detection_module.h>
#include <tiago_modules/tts_module.h>
#include <nen_modules/echo_module.h>
#include <nen_common_msgs/EchoCmdAction.h>
#include <log_modules/log_module.h>


#include <task2_recognition/Task2RecognitionConfig.h>
#include "person_definition.h"


// [publisher subscriber headers]

// [service client headers]

// [action server client headers]


typedef enum {
    T2_IDLE,
    T2_CHECK_KNOWN_PERSON,
    T2_CHECK_POSTMAN,
    T2_ASK_PERSON,
    T2_WAIT_ANSWER,
    T2_RETURN_VISITOR
} task2_recognition_states;

typedef enum{
    T2_RECOGNITION_RUNNING,
    T2_RECOGNITION_SUCCESS,
    T2_RECOGNITION_FAIL,
    T2_RECOGNITION_STOPPED
} task2_recognition_status;
/**
 * \brief IRI ROS Specific Algorithm Class
 *
 */
class CTask2Recognition : public CModule<task2_recognition::Task2RecognitionConfig>
{
  private:
    // [publisher attributes]

    // [subscriber attributes]

    // [service attributes]

    // [client attributes]

    // [action server attributes]

    // [action client attributes]

   /**
    * \brief config variable
    *
    * This variable has all the driver parameters defined in the cfg config file.
    * Is updated everytime function config_update() is called.
    */
    task2_recognition::Task2RecognitionConfig config_;

  //Modules


    //Face recognition module
    CFaceRecognitionModule face_recognition;
    //Shirt color detection module
    CShirtDetectionModule shirt_detection;
    //Text to speech module
    CTTSModule tts;
    //Amazon echo modules
    CEchoModule speech;
    nen_common_msgs::EchoCmdResult speech_command_;

    //Log module
    CLogModule logging;
    //Auxiliary variables to start task or ring bell from the dynamic_reconfigure
    bool start_recognition_;
    bool visitor_recognised_;
    bool cancel_pending_;
    bool people_stored_success_;
    //Variables for the delays
    Person current_person_;


    int current_action_retries_;
    int current_ask_retries_;
    //State machines
    task2_recognition_states state;

    task2_recognition_status status;

    /*!
       \brief Function that handles the tts module so that speaking is easier
       \param String with the sentence to be said by the robot
       \pre
       \post
       \return true if it ends saying the sentence, false otherwise.
    */
    bool ActionSaySentence(const std::string & sentence);

  protected:
    void state_machine(void);

    void reconfigure_callback(task2_recognition::Task2RecognitionConfig &config, uint32_t level);

   public:
     /**
      * \brief Constructor
      *
      * This constructor initializes specific class attributes and all ROS
      * communications variables to enable message exchange.
      */
      CTask2Recognition(const std::string &name, const std::string &name_space=std::string(""));



      bool AreFacesStored();

      void StartRecognition();

      Person GetCurrentPerson();

      bool IsVisitorRecognised();

      bool IsReady();

      bool StorePostmanAndKimble(const std::string & postman_path,const std::string & kimble_path);

      /**
       * \brief Stops the current execution of the module
       *
       * This function stops the execution of the state machine, stopping all sub-processes in execution.
       *
       */
      void stop(void);

      /**
      * \brief Indicates if there is any execution in process
      *
      * This function checks if the state machine is in execution, what it means it is searching
      * and bringing a object.
      *
      * \param return a bollean indicating wether the last goal has been ended or not.
      */
      bool is_finished(void);

      /**
        * \brief Indicates the current status of the module
        *
        * This function returns the current status of the execution, which is a defined value
        * from task3_subtasks_status_t. It is useful to check if the goal has been completed
        * successfully or not when the execution of the module has just finished.
        *
        * \return the current status of the module.
        *
        */
       task2_recognition_status get_status(void);

       task2_recognition_states get_state(void);

      /**
      * \brief Destructor
      *
      * This destructor frees all necessary dynamic memory allocated within this
      * this class.
      */
      ~CTask2Recognition(void);

};

#endif
