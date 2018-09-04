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

#ifndef _task2_recognition_alg_node_h_
#define _task2_recognition_alg_node_h_

#include <iri_base_algorithm/iri_base_algorithm.h>
#include "task2_recognition_client_alg.h"

#include <erl_classification_modules/person_classification_module.h>
#include <erl_classification_modules/person_comparison_module.h>
#include <erl_classification_modules/shirt_detection_module.h>
#include <tiago_modules/tts_module.h>
#include <nen_modules/echo_module.h>
#include <nen_common_msgs/EchoCmdAction.h>
#include <time.h>
// [publisher subscriber headers]

// [service client headers]

// [action server client headers]


typedef enum {
    T2_INIT,
    T2_CHECK_KNOWN_PERSON,
    T2_CHECK_POSTMAN,
    T2_ASK_PERSON,
    T2_WAIT_ANSWER,
    T2_VERIFY_ANSWER,
    T2_WAIT_VERIFY_ANSWER,
    T2_RETURN_VISITOR,
    } T2_RECOGNITION_STATES;


typedef enum {Deliman, Postman, Kimble, Plumber, Undefined} Person;

/**
 * \brief IRI ROS Specific Algorithm Class
 *
 */
class Task2RecognitionAlgNode : public algorithm_base::IriBaseAlgorithm<Task2RecognitionAlgorithm>
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
    Config config_;

  //Modules


    //Person classifier module
    CPersonClassificationModule classifier_module;
    //Shirt color detection module
    CShirtDetectionModule shirt_detection;
    //Text to speech module
    CTTSModule tts;
    //Amazon echo modules
    CEchoModule speech;
    nen_common_msgs::EchoCmdResult speech_command_;

    //Auxiliary variables to start task or ring bell from the dynamic_reconfigure
    bool start_recognition_;
    bool visitor_recognised_;

    //Variables for the delays
    Person current_person_;


    int current_action_retries;

    //State machines
    T2_RECOGNITION_STATES t2_m_s;



    /*!
       \brief Function that handles the tts module so that speaking is easier
       \param String with the sentence to be said by the robot
       \pre
       \post
       \return true if it ends saying the sentence, false otherwise.
    */
    bool ActionSaySentence(const std::string & sentence);


  public:
   /**
    * \brief Constructor
    *
    * This constructor initializes specific class attributes and all ROS
    * communications variables to enable message exchange.
    */
    Task2RecognitionAlgNode(void);

   /**
    * \brief Destructor
    *
    * This destructor frees all necessary dynamic memory allocated within this
    * this class.
    */
    ~Task2RecognitionAlgNode(void);



    void StartRecognition();

    Person GetCurrentPerson();

    bool IsVisitorRecognised();



  protected:
   /**
    * \brief main node thread
    *
    * This is the main thread node function. Code written here will be executed
    * in every node loop while the algorithm is on running state. Loop frequency
    * can be tuned by modifying loop_rate attribute.
    *
    * Here data related to the process loop or to ROS topics (mainly data structs
    * related to the MSG and SRV files) must be updated. ROS publisher objects
    * must publish their data in this process. ROS client servers may also
    * request data to the corresponding server topics.
    */
    void mainNodeThread(void);

   /**
    * \brief dynamic reconfigure server callback
    *
    * This method is called whenever a new configuration is received through
    * the dynamic reconfigure. The derivated generic algorithm class must
    * implement it.
    *
    * \param config an object with new configuration from all algorithm
    *               parameters defined in the config file.
    * \param level  integer referring the level in which the configuration
    *               has been changed.
    */
    void node_config_update(Config &config, uint32_t level);

   /**
    * \brief node add diagnostics
    *
    * In this abstract function additional ROS diagnostics applied to the
    * specific algorithms may be added.
    */
    void addNodeDiagnostics(void);

    // [diagnostic functions]

    // [test functions]
};

#endif
