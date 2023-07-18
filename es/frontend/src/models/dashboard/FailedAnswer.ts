import {QuestionAnswer} from '@/models/management/QuestionAnswer';
import {ISOtoString} from '@/services/ConvertDateService';

export default class FailedAnswer {
    id!: number;
    questionAnswerDto!: QuestionAnswer;
    answered!: boolean;
    questionName!: string;
    collected!: string;


    constructor(jsonObj?: FailedAnswer) {
        if (jsonObj) {
            this.id = jsonObj.id;
            this.questionAnswerDto = new QuestionAnswer(jsonObj.questionAnswerDto);
            this.questionName = this.questionAnswerDto.question.content;
            this.answered = jsonObj.answered;
            this.collected = ISOtoString(jsonObj.collected);
        }
    }

    static failedAnswerHeader = [
        {
            text: 'Actions',
            value: 'action',
            align: 'left',
            width: '5px',
            sortable: false,
        },
        {
            text: 'Question',
            value: 'questionName',
            width: '150px',
            align: 'center',
        },

        {
            text: 'Answered',
            value: 'answered',
            width: '65%',
            align: 'right',
            sortable: false,
        },
        {
            text: 'Collected',
            value: 'collected',
            width: '20px',
            align: 'center',
        },
    ];
}
