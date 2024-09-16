#pragma once

void init_irq();
void unmask_irq(int IRQ);
void mask_irq(int IRQ);
void end_of_interrupt();