#include "task2_recognition_client_alg_node.h"

Task2RecognitionAlgNode::Task2RecognitionAlgNode(void) :
algorithm_base::IriBaseAlgorithm<Task2RecognitionAlgorithm>(),
classifier_module("classifier",ros::this_node::getName()),
tts("tts_module",ros::this_node::getName()),
speech("echo_module",ros::this_node::getName()),
shirt_detection("shirt_color_detection_module",ros::this_node::getName())
{
  this->t2_m_s =  T2_INIT;
  this->current_person_ = Undefined;
}

Task2RecognitionAlgNode::~Task2RecognitionAlgNode(void)
{
  // [free dynamic memory]
}

bool Task2RecognitionAlgNode::ActionSaySentence(const std::string & sentence){
  static bool is_sentence_sent = false;
  if (!is_sentence_sent){
    tts.say(sentence);
    is_sentence_sent = true;
  }
  else {
    if (tts.is_finished()){
      if (tts.get_status()==TTS_MODULE_SUCCESS or this->current_action_retries >= this->config_.max_action_retries){
        is_sentence_sent  = false;
        this->current_action_retries = 0;
        return true;

      }
      else {
        ROS_INFO ("[TASK2] TTS module finished unsuccessfully. Retrying");
        is_sentence_sent  = false;
        this->current_action_retries ++;
        return false;
      }

    }
  }
  return false;
}


void Task2RecognitionAlgNode::StartRecognition(){
  this->start_recognition_ = true;
}

bool Task2RecognitionAlgNode::IsVisitorRecognised(){
  return this->visitor_recognised_;
}


Person Task2RecognitionAlgNode::GetCurrentPerson(){
  if (IsVisitorRecognised()){
    return this->current_person_;
  }
  return Undefined;
}

void Task2RecognitionAlgNode::mainNodeThread(void)
{
  std::string label;
  float acc;
  std::string error_msg;
  bool result;
  int color;

  switch (this->t2_m_s){
    case T2_INIT:
      ROS_INFO("[TASK2] Wait start");
      if(this->start_recognition_){
        this->start_recognition_ = false;
        this->t2_m_s = T2_CHECK_KNOWN_PERSON;
        this->visitor_recognised_ = false;
      }
      else
      this->t2_m_s=T2_INIT;
      break;


    case T2_CHECK_KNOWN_PERSON:

      result = this->classifier_module.classify_current_person(label,acc,error_msg);
      if (result && label == this->config_.person_kimble){
        this->current_person_ = Kimble;
        this->t2_m_s = T2_RETURN_VISITOR;
      }
      else {
        if (result && label == this->config_.person_postman){
          this->current_person_ = Postman;
          this->t2_m_s = T2_RETURN_VISITOR;

        }
        else {
          this->t2_m_s = T2_CHECK_POSTMAN;
        }
      }

      break;


    case T2_CHECK_POSTMAN:
      color = this->shirt_detection.GetShirtColor();

      if (color == config_.color_yellow_id){
        this->current_person_ = Postman;
        this->t2_m_s = T2_RETURN_VISITOR;
      }
      else {
        this->t2_m_s = T2_ASK_PERSON;
      }
      break;

    case T2_ASK_PERSON:
      if (this->ActionSaySentence(this->config_.sentence_ask_person)){
        this->t2_m_s = T2_WAIT_ANSWER;
        this->speech.listen();
      }
      break;


    case T2_WAIT_ANSWER:
      //TODO : AMAZON ALEXA
      if (this->speech.is_finished()){
        if (this->speech.get_status()==ECHO_MODULE_SUCCESS){
          this->speech_command_ = this->speech.get_result();
          if (this->speech_command_.cmd.cmd_id == this->config_.speech_plumber_id){
            this->current_person_ = Plumber;
            this->t2_m_s = T2_VERIFY_ANSWER;
          }
          else if (this->speech_command_.cmd.cmd_id == this->config_.speech_deliman_id){
            this->current_person_ = Deliman;
            this->t2_m_s = T2_VERIFY_ANSWER;
          }
          else {
            this->t2_m_s = T2_ASK_PERSON;

          }
        }
        else {
          this->t2_m_s = T2_ASK_PERSON;

        }
      }
      else {
        this->t2_m_s = T2_WAIT_ANSWER;
      }

      break;
    case T2_VERIFY_ANSWER:
      if (this->current_person_ == Deliman){
        if (this->ActionSaySentence(this->config_.sentence_verify_deliman)){
          this->t2_m_s = T2_WAIT_VERIFY_ANSWER;
          this->speech.listen();
        }
      }
      else {
        if (this->current_person_ == Plumber){
          if (this->ActionSaySentence(this->config_.sentence_verify_plumber)){
            this->t2_m_s = T2_WAIT_VERIFY_ANSWER;
            this->speech.listen();
          }
        }
      }

      break;

    case T2_WAIT_VERIFY_ANSWER:
    //TODO:AMAZON ALEXA
      {
        if (this->speech.is_finished()){
          if (this->speech.get_status()==ECHO_MODULE_SUCCESS){
            this->speech_command_ = this->speech.get_result();
            if (this->speech_command_.cmd.cmd_id == this->config_.speech_yes_id){

              this->t2_m_s = T2_RETURN_VISITOR;
            }
            else if (this->speech_command_.cmd.cmd_id == this->config_.speech_no_id){

              if (current_person_ == Deliman) current_person_ = Plumber;
              else if (current_person_ == Plumber) current_person_ = Deliman;
              this->t2_m_s = T2_VERIFY_ANSWER;
            }
            else {
              this->t2_m_s = T2_ASK_PERSON;

            }
          }
          else {
            this->t2_m_s = T2_ASK_PERSON;

          }
        }
        else {
          this->t2_m_s = T2_WAIT_ANSWER;
        }
        break;
      }

    case T2_RETURN_VISITOR:
    {
      this->visitor_recognised_ = true;
      this->t2_m_s = T2_INIT;
      break;
    }

  }
  // [fill srv structure and make request to the server]

  // [fill action structure and make request to the action server]

  // [publish messages]
}

/*  [subscriber callbacks] */

/*  [service callbacks] */

/*  [action callbacks] */

/*  [action requests] */

void Task2RecognitionAlgNode::node_config_update(Config &config, uint32_t level)
{
  this->alg_.lock();

  this->config_=config;

  this->alg_.unlock();
}

void Task2RecognitionAlgNode::addNodeDiagnostics(void)
{
}

/* main function */
int main(int argc,char *argv[])
{
  return algorithm_base::main<Task2RecognitionAlgNode>(argc, argv, "erl_task2_alg_node");
}
