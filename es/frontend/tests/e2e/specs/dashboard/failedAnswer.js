describe('Student Failed Answers Access', () => {
    beforeEach(() => {
        cy.deleteQuestionsAndAnswers();
        //create quiz
        cy.demoTeacherLogin();
        cy.createQuestion(
            'Question Title',
            'Question',
            'ChooseThisWrong',
            'Option',
            'Option',
            'Correct'
        );
        cy.createQuestion(
            'Question Title2',
            'Question',
            'Option',
            'Option',
            'Correct',
            'ChooseThisWrong'

        );
        cy.createQuizzWith2Questions(
            'Quiz Title',
            'Question Title',
            'Question Title2'
        );
        cy.contains('Logout').click();
    });

    afterEach(() => {
        cy.deleteFailedAnswers();
        cy.deleteQuestionsAndAnswers();
    });

    it('student check failed answers', () => {
        cy.intercept('GET', '**/students/dashboards/executions/*').as('getDashboard');
        cy.intercept('GET', '**/students/failedanswers/dashboard/*').as('getFailedAnswers');
        cy.intercept('PUT', '**/students/updatefailedanswers/dashboard/*').as('updateFailedAnswers');
        cy.intercept('DELETE', '**/students/failedanswers/failedanswer/*').as('deleteFailedAnswer');

        cy.demoStudentLogin();
        cy.solveToFailQuizz('Quiz Title', 2);

        cy.get('[data-cy="dashboardMenuButton"]').click();
        cy.wait('@getDashboard');

        cy.get('[data-cy="failedAnswersMenuButton"]').click();
        cy.wait('@getFailedAnswers');

        cy.get('[data-cy="refreshFailedAnswerButton"]').click();
        cy.wait('@updateFailedAnswers');
        cy.wait('@getFailedAnswers');

        cy.get('[data-cy="showStudentViewDialog"]')
            .should('have.length.at.least', 1)
            .first()
            .click();
        cy.contains('close').scrollIntoView().click();

        cy.get('[data-cy="deleteFailedAnswerButton"]')
            .should('have.length.at.least', 1)
            .first()
            .click();
        cy.wait('@deleteFailedAnswer');
        cy.closeErrorMessage();

        cy.setFailedAnswersAsOld();
        cy.get('[data-cy="refreshFailedAnswerButton"]').click();
        cy.wait('@updateFailedAnswers');

        cy.get('[data-cy="deleteFailedAnswerButton"]')
            .should('have.length.at.least', 1)
            .first()
            .click();
        cy.wait('@deleteFailedAnswer');

        cy.contains('Logout').click();
        Cypress.on('uncaught:exception', (err, runnable) => {
            // returning false here prevents Cypress from
            // failing the test
            return false;
        });
    });


});
