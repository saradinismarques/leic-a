describe('Student Walkthrough', () => {
    beforeEach(() => {
        //create quiz
        cy.deleteQuestionsAndAnswers();
        cy.deleteWeeklyScores();
        cy.demoTeacherLogin();
        cy.createQuestion(
            'Question Title',
            'Question',
            'Option',
            'Option',
            'Option',
            'Correct'
        );
        cy.createQuestion(
            'Question Title2',
            'Question',
            'Option',
            'Option',
            'Option',
            'Correct'
        );
        cy.createQuizzWith2Questions(
            'Quiz Title',
            'Question Title',
            'Question Title2'
        );
        cy.contains('Logout').click();
    });

    it('student creates discussion', () => {
        cy.demoStudentLogin();
        cy.solveQuizz('Quiz Title', 2);
        cy.contains('Logout').click();
        Cypress.on('uncaught:exception', (err, runnable) => {
            // returning false here prevents Cypress from
            // failing the test
            return false;
        });

    });

    it('create weekly scores', () => {
        cy.demoStudentLogin();

        cy.route('GET', '/students/dashboards/executions/*').as('getDashboard');

        cy.get('[data-cy="dashboardMenuButton"]').click();
        cy.wait('@getDashboard').its('status').should('eq', 200);

        cy.route('GET', '/weeklyScores/dashboard/*').as('getWeeklyScores');
        cy.get('[data-cy="weeklyScoresMenuButton"]').click();
        cy.wait('@getWeeklyScores');

        cy.route('PUT', '/students/updateweeklyscore/dashboard/*').as('updateWeeklyScores');
        cy.get('[data-cy="refreshWeeklyScoresMenuButton"]').click();
        cy.wait('@updateWeeklyScores');


        cy.route('DELETE', '/students/weeklyScores/weeklyscore/*').as('deleteWeeklyScores');
        cy.get('[data-cy="deleteWeeklyScoreButton"]').click();
        cy.wait('@deleteWeeklyScores');

        cy.closeErrorMessage();

        cy.createWeeklyScore();

        cy.route('PUT', '/students/updateweeklyscore/dashboard/*').as('updateWeeklyScores');
        cy.get('[data-cy="refreshWeeklyScoresMenuButton"]').click();
        cy.wait('@updateWeeklyScores');

        cy.route('DELETE', '/students/weeklyScores/weeklyscore/*').as('deleteWeeklyScores');
        cy.get('[data-cy="deleteWeeklyScoreButton"]').should((list) =>
            expect(list).to.have.length(2)).eq(1).click();
        cy.wait('@deleteWeeklyScores');



        cy.contains('Logout').click();
        Cypress.on('uncaught:exception', (err, runnable) => {
            // returning false here prevents Cypress from
            // failing the test
            return false;
        });
    });


    afterEach(() => {
        cy.deleteQuestionsAndAnswers();
        cy.deleteWeeklyScores();
    });

});