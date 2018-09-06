#include "task2_recognition.h"

CTask2Recognition::CTask2Recognition(const std::string &name, const std::string &name_space) : CModule(name,name_space),
face_recognition("face_recognition", this->module_nh.getNamespace()),
tts("tts_module",this->module_nh.getNamespace()),
speech("echo_module",this->module_nh.getNamespace()),
shirt_detection("shirt_color_detection_module",this->module_nh.getNamespace())
{
  this->start_operation();
  this->state =  T2_INIT_RECOGNITION;
  this->current_person_ = Undefined;
  this->current_action_retries_ = 0;
  this->start_recognition_ = false;
  this->cancel_pending_ = false;

  //TODO : When initializing we need to call StorePostmanAndKimble from main node



}

CTask2Recognition::~CTask2Recognition(void)
{
  // [free dynamic memory]
}
bool CTask2Recognition::StorePostmanAndKimble(const std::string & postman_path,const std::string & kimble_path){
    std::cout << "Storing : "<<postman_path<<" "<<kimble_path << '\n';
    bool success_store_postman = this->face_recognition.StorePersonFromPath(postman_path, this->config_.person_postman);
    bool success_store_kimble = this->face_recognition.StorePersonFromPath(kimble_path, this->config_.person_kimble);
    return success_store_postman && success_store_kimble;
}
bool CTask2Recognition::ActionSaySentence(const std::string & sentence){
  static bool is_sentence_sent = false;
  if (!is_sentence_sent){
    tts.say(sentence);
    is_sentence_sent = true;
  }
  else {
    if (tts.is_finished()){
      if (tts.get_status()==TTS_MODULE_SUCCESS or this->current_action_retries_ >= this->config_.max_action_retries){
        is_sentence_sent  = false;
        this->current_action_retries_ = 0;
        return true;

      }
      else {
        ROS_INFO ("[TASK2] TTS module finished unsuccessfully. Retrying. Total retries = %d of %d", this->current_action_retries_, this->config_.max_action_retries);
        is_sentence_sent  = false;
        this->current_action_retries_ ++;
        return false;
      }

    }
  }
  return false;
}


void CTask2Recognition::StartRecognition(){
  this->start_recognition_ = true;
}

bool CTask2Recognition::IsVisitorRecognised(){
  return this->visitor_recognised_;
}


Person CTask2Recognition::GetCurrentPerson(){
  if (IsVisitorRecognised()){
    return this->current_person_;
  }
  return Undefined;
}

void CTask2Recognition::state_machine(void)
{
  std::string label;
  int color;
  if (this->cancel_pending_){
      this->state = T2_END_RECOGNITION;
      this->cancel_pending_ = false;
  }

  switch (this->state){
    case T2_INIT_RECOGNITION:
      if(this->start_recognition_){
        ROS_INFO("[TASK2Recognition]  Starting");
        this->start_recognition_ = false;
        this->state = T2_CHECK_KNOWN_PERSON;
        this->visitor_recognised_ = false;
      }
      else
        this->state=T2_INIT_RECOGNITION;
      break;


    case T2_CHECK_KNOWN_PERSON:
      label = this->face_recognition.GetCurrentPerson();
      ROS_INFO("[TASK2Recognition] Check for known person");
      std::cout << label << '\n';
      if (label == this->config_.person_kimble){
        this->current_person_ = Kimble;
        this->state = T2_RETURN_VISITOR;
      }
      else {
        if (label == this->config_.person_postman){
          this->current_person_ = Postman;
          this->state = T2_RETURN_VISITOR;
        }
        else {
          this->state = T2_CHECK_POSTMAN;
        }
      }

      break;


    case T2_CHECK_POSTMAN:
      ROS_INFO("[TASK2Recognition] Check for shirt color");
      color = this->shirt_detection.GetShirtColor();

      if (color == config_.color_yellow_id){
        this->current_person_ = Postman;
        this->state = T2_RETURN_VISITOR;
      }
      else {
        this->state = T2_ASK_PERSON;
      }
      break;

    case T2_ASK_PERSON:
      ROS_INFO ("[TASK2Recognition] Asking who the person is");
      if (this->ActionSaySentence(this->config_.sentence_ask_person)){
        this->state = T2_WAIT_ANSWER;
        this->speech.listen();
      }
      break;


    case T2_WAIT_ANSWER:
      //TODO : AMAZON ALEXA
      ROS_INFO("[Task2Recognition] Waiting answer from amazon alexa");
      if (this->speech.is_finished()){
        if (this->speech.get_status()==ECHO_MODULE_SUCCESS){
          this->speech_command_ = this->speech.get_result();
          if (this->speech_command_.cmd.cmd_id == this->config_.speech_plumber_id){
            this->current_person_ = Plumber;
            this->state = T2_VERIFY_ANSWER;
          }
          else if (this->speech_command_.cmd.cmd_id == this->config_.speech_deliman_id){
            this->current_person_ = Deliman;
            this->state = T2_VERIFY_ANSWER;
          }
          else {
            this->state = T2_ASK_PERSON;

          }
        }
        else {
          this->state = T2_ASK_PERSON;

        }
      }
      else {
        this->state = T2_WAIT_ANSWER;
      }

      break;
    case T2_VERIFY_ANSWER:
      if (this->current_person_ == Deliman){
        if (this->ActionSaySentence(this->config_.sentence_verify_deliman)){
          this->state = T2_WAIT_VERIFY_ANSWER;
          this->speech.listen();
        }
      }
      else {
        if (this->current_person_ == Plumber){
          if (this->ActionSaySentence(this->config_.sentence_verify_plumber)){
            this->state = T2_WAIT_VERIFY_ANSWER;
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

              this->state = T2_RETURN_VISITOR;
            }
            else if (this->speech_command_.cmd.cmd_id == this->config_.speech_no_id){

              if (current_person_ == Deliman) current_person_ = Plumber;
              else if (current_person_ == Plumber) current_person_ = Deliman;
              this->state = T2_VERIFY_ANSWER;
            }
            else {
              this->state = T2_ASK_PERSON;

            }
          }
          else {
            this->state = T2_ASK_PERSON;

          }
        }
        else {
          this->state = T2_WAIT_ANSWER;
        }
        break;
      }

    case T2_RETURN_VISITOR:
    {
      this->visitor_recognised_ = true;
      this->state = T2_END_RECOGNITION;
      break;
    }
    case T2_END_RECOGNITION:
	ROS_INFO("Visitor recognised: ");
	std::cout<<this->current_person_<<std::endl;

        break;

  }
}

void CTask2Recognition::reconfigure_callback(task2_recognition::Task2RecognitionConfig &config, uint32_t level)
{
  ROS_INFO("CTask2Recognition: reconfigure callback");
  this->lock();
  this->config_=config;

  if (this->config_.store_postman_kimble){
      this->config_.store_postman_kimble = false;
      this->StorePostmanAndKimble(this->config_.postman_image_path,this->config_.kimble_image_path);
  }
  /* set the module rate */
//  this->set_rate(config.rate_hz);
  this->unlock();
}

void CTask2Recognition::stop(void){
    this->cancel_pending_ = true;
}

bool CTask2Recognition::is_finished(void){
    return IsVisitorRecognised();
}

task2_recognition_states CTask2Recognition::get_state(void){
    return this->state;
}
