import { ISOtoString } from '@/services/ConvertDateService';
import Question from '../management/Question';

export default class DifficultQuestion {
  id!: number;
  percentage!: number;
  removed!: boolean;
  removedDate!: string;

  questionDto!: Question;

  constructor(jsonObj?: DifficultQuestion) {
    if (jsonObj) {
      this.id = jsonObj.id;
      this.percentage = jsonObj.percentage;
      if (jsonObj.removed) this.removed = jsonObj.removed;
      if (jsonObj.removedDate)
        this.removedDate = ISOtoString(jsonObj.removedDate);
      if (jsonObj.questionDto)
        this.questionDto = new Question(jsonObj.questionDto);
    }
  }
}
