import { ISOtoString } from '@/services/ConvertDateService';

export default class WeeklyScore {
    id!: number;
    numberAnswered!: number;
    uniquelyAnswered!: number;
    percentageCorrect!: number;
    week!: string;
    closed!: boolean;

    constructor(jsonObj?: WeeklyScore) {

        if (jsonObj) {
            this.id = jsonObj.id;
            this.numberAnswered = jsonObj.numberAnswered;
            this.uniquelyAnswered = jsonObj.uniquelyAnswered;
            this.percentageCorrect = jsonObj.percentageCorrect;

            if (jsonObj.week)
                this.week = ISOtoString(jsonObj.week);
            if (jsonObj.closed)
                this.closed = jsonObj.closed;
        }
    }
}